#include "stdafx.h"

/*
* default::32bit per.
* PipelineStateObject => statestore. PSO
* Rendering is based on shader, not on object.
IA	rootsignature::64.	srv::2.	=> SV_VertexID // SV_InstanceID // SV_PrimitiveID
VS	input::16.	output::16.
HS
TS
DS
GS			(SO)->OUTPUT
RS	Rasterize	=> SV_IsFrontFace
	<= SV_Position
PS	input::(GS)?(32):(16). Can_Discard	=> SV_Depth // nostencil
OM

class Shader == PSO.
PSO = D3D12_GRAPHICS_PIPELINE_STATE_DESC. PS,Vs,INPUT_RAYOUT...etc

// drawing with shaders only. do not use mesh or object directly. => easy 2 use and ggalggm.
*/