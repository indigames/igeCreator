#pragma once

#include "pyxieTypes.h"

namespace pyxie
{
	class pyxieShaderDescriptor;

	class PYXIE_EXPORT pyxieFigureExportConfigManager {
	public:
		enum {
			BASE_SCALE,
			EXPORT_NAMES,
			TRIANGLE_STRIP,
			INHERIT_JOINT_NAME,
			CLOP_JOINT,
			GEN_MIPMAP,
			KILL_MIPMAP,
			FREEZE_GEOMETORY,
			COMPUTE_PERIOD,
			MATERIAL_PARAM_SPARE,
			OPTIMIZE_MESH,
			OPTIMIZE_VERTEX,
			OPTIMIZE_ANIMATION,
			SHADER_NUM_DIR_LAMP,
			SHADER_NUM_POINT_LAMP,
			SHADER_NUM_SPOT_LAMP,
			SHADER_AMBIENT_TYPE,
			SHADER_UV_OFFSET_SET0,
			SHADER_UV_OFFSET_SET1,
			SHADER_UV_OFFSET_SET2,
			SHADER_UV_SCROLL_SET0,
			SHADER_UV_SCROLL_SET1,
			SHADER_UV_SCROLL_SET2,
			SHADER_PROJECTION_MAPPING_SET0,
			SHADER_PROJECTION_MAPPING_SET1,
			SHADER_PROJECTION_MAPPING_SET2,
			SHADER_MAKE_SHADOW,
			SHADER_RECEIVE_SHADOW,
			SHADER_DEPTH_SHADOW,
			SHADER_DISTORTION,
			SHADER_COLOR_TEXTURE,
			SHADER_NORMAL_TEXTURE,
			SHADER_LIGHT_TEXTURE,
			SHADER_OVERLAY_COLOR_TEXTURE,
			SHADER_OVERLAY_NORMAL_TEXTURE,
			SHADER_COLOR_TEXTURE_RGB_UV_SET,
			SHADER_NORMAL_TEXTURE_RGB_UV_SET,
			SHADER_LIGHT_TEXTURE_RGB_UV_SET,
			SHADER_OVERLAY_COLOR_TEXTURE_RGB_UV_SET,
			SHADER_OVERLAY_NORMAL_TEXTURE_RGB_UV_SET,
			SHADER_COLOR_TEXTURE_ALPHA_UV_SET,
			SHADER_NORMAL_TEXTURE_ALPHA_UV_SET,
			SHADER_LIGHT_TEXTURE_ALPHA_UV_SET,
			SHADER_OVERLAY_COLOR_TEXTURE_ALPHA_UV_SET,
			SHADER_OVERLAY_NORMAL_TEXTURE_ALPHA_UV_SET,
			SHADER_ALPHA_MAP,
			SHADER_SPECULAR,
			SHADER_SPECULAR_MAP,
			SHADER_VERTEX_COLOR,
			SHADER_VERTEX_ALPHA,
			RENDER_STATE_ALPHA_TEST,
			RENDER_STATE_ALPHA_BLEND,
			RENDER_STATE_ALPHA_OP,
			RENDER_STATE_DOUBLESIDE,
			RENDER_STATE_FRONT_FACE_CULLING,
			RENDER_STATE_Z_TEST,
			RENDER_STATE_Z_WRITE,
			RENDER_STATE_COLOR_MASK,
			RENDER_STATE_SCISSOR_TEST,
		};

		pyxieFigureExportConfigManager();
	public:
		static pyxieFigureExportConfigManager& Instance();
		void ClearOption();
		void SetOptionString(const char* name, const char* value);
		void SetOptionInt(const char* name, int value);
		void SetOptionFloat(const char* name, float value);
		float BaseScale();
		bool ExportNames();
		bool TriangleStrip();
		bool InheritJointName();
		bool ClopJoint();
		bool FreezeGeometory();
		bool ComputePeriod();
		int MaterialParamSpare();
		bool OptimizeMesh();
		bool OptimizeVertex();
		bool OptimizeAnimation();

		void UpdateShaderOption(pyxieShaderDescriptor &desc);
	};

/*
	class PYXIE_EXPORT pyxieFigureExportConfigManager {
		bool	TriangleStrip;				//Convert vertices to triangle strip format
		bool	FreezeGeometoryTransform;	//ジオメトリのトランスフォームをフリーズする
		bool	FullPathName;				//ノード名をルートからの親の名前を'/'でつなげた名前に変更する
		bool	OutputNameList;				//jointsとmaterialのノード名を出力する
		bool	OutputNotes;				//jointsとmaterialのノード名を出力する
		bool	GenMipmap;					//テクスチャのミップマップを生成する
		bool	ClopTransform;				//スキンウェイトを持ったオブジェクトノードのトランスフォームは削除する
		bool	ComputePeriod;
		float	BaseScale;
		float	Tolerance;
		pyxieFigureExportConfigManager();
	public:
		static pyxieFigureExportConfigManager& Instance();

		void ReadConfigFiles(const char* filePath);

		bool	IsTriangleStrip() { return TriangleStrip; }
		bool	IsFreezeGeometoryTransform() { return FreezeGeometoryTransform; }
		bool	IsFullPathName() { return FullPathName; }
		bool	IsOutputNameList() { return OutputNameList; }
		bool	IsOutputNotes() { return OutputNotes; }
		bool	IsGenMipmap() { return GenMipmap; }
		bool	IsClopTransform() { return ClopTransform; }
		bool	IsComputePeriod() { return ComputePeriod; }
		float	GetBaseScale() { return BaseScale; }
		float	GetTolerance() { return Tolerance; }
		void	SetBaseScale(float v) { BaseScale = v; }
	};
*/
}
