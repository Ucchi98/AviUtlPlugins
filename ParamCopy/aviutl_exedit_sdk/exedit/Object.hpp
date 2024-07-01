#pragma once
#include <cstdint>
#include <Windows.h>

#include <aviutl/flag.hpp>

namespace ExEdit {
	struct Object {
		static constexpr size_t MAX_DISPNAME = 64;
		static constexpr size_t MAX_FILTER = 12;
		static constexpr size_t MAX_TRACK = 64;
		static constexpr size_t MAX_CHECK = 48;

		enum class Flag : uint32_t {
			Exist     = 1 << 0, // ��������ΑS��1


			Clipping  = 1 << 8, // ��̃I�u�W�F�N�g�ŃN���b�s���O
			Camera    = 1 << 9, // �J��������̑Ώ�

			Media     = 0,       // ���f�B�A�I�u�W�F�N�g
			Filter    = 1 << 16, // �t�B���^�I�u�W�F�N�g Filter::Flag::Output|Filter::Flag::Input

			Image     = 0,       // �摜�ւ̃t�B���^
			Sound     = 1 << 17, // �����ւ̃t�B���^ Filter::Flag::Audio
			Effect    = 1 << 18, // ���f�B�A�I�u�W�F�N�g�̃t�B���^���� Filter::Flag::Effect

			Control   = 1 << 19, // �O���[�v����A�J��������(�J�����G�t�F�N�g�܂�)�A���Ԑ��� Filter::Flag::Control
			ShowRange = 1 << 20, // �Ώۃ��C���[�͈͂��^�C�����C����ɕ\������� Filter::ExdataUse[0]::name=="range"

			// 100337b5 (?)
			bit31 = 1U << 31,
		} flag;

		int32_t layer_disp;
		int32_t frame_begin;
		int32_t frame_end;
		char dispname[MAX_DISPNAME];
		int32_t index_midpt_leader;

		struct FilterParam {
			static const int32_t None = -1;
			int32_t id;
			int16_t track_begin;
			int16_t check_begin;
			DWORD exdata_offset;
			constexpr bool is_valid()const { return id>=0; }
		} filter_param[MAX_FILTER];

		enum class FilterStatus : unsigned char {
			Active  = 1 << 0,
			Folding = 1 << 1,
			Gui     = 1 << 2, Unknown = 1 << 2,
		} filter_status[MAX_FILTER];

		int16_t track_n;
		int16_t check_n;
		DWORD exdata_size;
		int32_t track_value_left[MAX_TRACK];
		int32_t track_value_right[MAX_TRACK];

		struct TrackMode {
			static const int16_t isScript = 0x0f;
			static const int16_t isDecelerate = 0x20;
			static const int16_t isAccelerate = 0x40;

			int16_t num;
			int16_t script_idx;
		} track_mode[MAX_TRACK];

		int32_t check_value[MAX_CHECK];
		DWORD exdata_offset;
		int32_t group_belong;
		int32_t track_param[MAX_TRACK];
		int32_t layer_set;
		int32_t scene_set;

		int32_t countFilters() const {
			int32_t ret = 0;
			for (const auto& v : this->filter_param) {
				if(!v.is_valid()) break;
				ret++;
			}
			return ret;
		}
	};
	
}
template<>struct AviUtl::detail::flag::ops_def<ExEdit::Object::Flag>:std::true_type{};
template<>struct AviUtl::detail::flag::ops_def<ExEdit::Object::FilterStatus>:std::true_type{};
