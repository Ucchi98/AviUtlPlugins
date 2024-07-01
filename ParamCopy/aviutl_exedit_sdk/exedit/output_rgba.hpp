#pragma once
#include <cstdint>
#include <stdexcept>
#include <Windows.h>

#include <exedit/pixel.hpp>

namespace ExEdit {
    struct OutputData {
        int32_t frame_begin;
        int32_t frame_end;
        int32_t width;
        int32_t height;
        int32_t rate;
        int32_t scale;
    };

    struct OutputFactory {
        BOOL (*output_start)(OutputData* data);
        void (*output_end)();
        PixelBGRA* (*get_image)(int32_t frame);
    };

    class OutputHelper {
        OutputFactory factory;

    public:
        OutputData data;

        OutputHelper() {
            const auto exedit_map_ver = 0x3031; // '01'
            char name[256];
            wsprintfA(name, "exedit_%d_%d", exedit_map_ver, GetCurrentProcessId());
            auto hfmo = OpenFileMappingA(FILE_MAP_WRITE, FALSE, name);
            if (!hfmo) throw std::runtime_error("�g���ҏW�v���O�C���̃o�[�W�������Ⴄ���F������Ă��܂���");

            auto mapping = MapViewOfFile(hfmo, FILE_MAP_WRITE, 0, 0, sizeof(factory));
            if(!mapping) {
                CloseHandle(hfmo);
                throw std::runtime_error("�g���ҏW�v���O�C���̃o�[�W�������Ⴄ���F������Ă��܂���");
            }
            memcpy(&factory, mapping, sizeof(factory));
            UnmapViewOfFile(mapping);
            CloseHandle(hfmo);

            auto start_ret = factory.output_start(&data);
            if (start_ret == FALSE) throw std::runtime_error("�g���ҏW�v���O�C���̏�񂪎擾�ł��܂���");
        }

        ~OutputHelper() {
            factory.output_end();
        }

        auto get_image(int32_t frame) {
            auto ret = factory.get_image(frame);
            if(!ret) throw std::runtime_error("�g���ҏW�v���O�C������摜�̎擾�Ɏ��s���܂���");
            return ret;
        }

        auto operator->() { return &data; }
    };
}
