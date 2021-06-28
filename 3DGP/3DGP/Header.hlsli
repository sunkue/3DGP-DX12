



float cellSmooth(float x)
{
	const int slice = 3;
	const float t = (1.0f / slice);
	float ret = 0.0f;
	for (int i = 0; i < slice; i++)
	{
		ret += smoothstep(t * i, t * (i + 1), x) * (t * (i + 1));
	}
	return ret;
}

float cellStatic(float x)
{
	const int slice = 3;
	return ceil(x * slice) / slice;
}
float factorMode(int mode, float x)
{
	switch (mode)
	{
		case 1:
			x = cellStatic(x);
			break;
		case 2:
			x = cellSmooth(x);
			break;
		default:
			break;
	}
	return x;
}