#pragma once
#include <engine/model.h>
class Art
{
private:
	int recolectar;
	float rotation;
	float Nescala;
	float _esca;
	float velPosi;
	float Posi;

public:	
	vec3 posiBase;
	Model _mod;
	Art(Model _mod);
	~Art();
	void DrawArt(Shader shader);
	void setArt(int _valor);
	int getArt();
	void ArteSanima();
};

