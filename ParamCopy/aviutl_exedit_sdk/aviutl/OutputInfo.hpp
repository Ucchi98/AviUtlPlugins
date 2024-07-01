#pragma once
#include <cstdint>
#include <Windows.h>

#include <aviutl/flag.hpp>

namespace AviUtl{
    struct OutputInfo {
        enum class Flag : uint32_t {
            Video = 1 << 0, // �摜�f�[�^����
            Audio = 1 << 1, // �����f�[�^����
            Batch = 1 << 2, // �o�b�`�o�͒�
        } flag;

        int32_t w, h; // �c���T�C�Y
        int32_t rate, scale; // �t���[�����[�g
        int32_t n;
        int32_t size;
        int32_t audio_rate;
        int32_t audio_ch;
        int32_t audio_n;
        int32_t audio_size;
        const char* savefile;

        void* (*func_get_video)(int32_t frame);
        void* (*func_get_audio)(int32_t start, int32_t length, int32_t* readed);

        BOOL (*func_is_abort)();
        BOOL (*func_rest_time_disp)(int32_t now, int32_t total);
        
        enum class FrameFlag : uint32_t {
            KeyFrame  = 1 << 0,
            CopyFrame = 1 << 1,
        };
        int32_t (*func_get_flag)(FrameFlag frame);

        BOOL (*func_update_preview)();
        void* (*func_get_video_ex)(int32_t frame, DWORD format);
    };
    template<> struct detail::flag::ops_def<OutputInfo::Flag>: std::true_type {};
    template<> struct detail::flag::ops_def<OutputInfo::FrameFlag>: std::true_type {};
}
