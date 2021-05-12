#pragma once
class Vertex
{
	XMFLOAT3A mxmf3Pos;
};

class DiffusedVertex :public Vertex
{
	XMFLOAT4 mxmf4Diffuse;
};