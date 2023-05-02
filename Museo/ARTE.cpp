#include "ARTE.h"
Art::Art(Model _mod) {
	this->_mod =_mod;
	this->_esca = 0.002;
	this->rotation = 0;
	this->Posi = 0;
	this->velPosi = 0.005;
	this->posiBase = this->_mod.getPosition();
}

Art::~Art() {
}

void Art::DrawArt(Shader shader){
	this->_mod.Draw(shader);
}

void Art::setArt(int _valor) {
	this->recolectar += _valor;
}
int Art::getArt() {
	return this->recolectar;
}

void Art::ArteSanima() {
	if (this->Nescala < 0.2f || this->Nescala >0.25f) {
		this->Nescala = this->Nescala > 0.25f ? 0.25f : 0.2f;
		this->_esca *= -1;
	}
	this->Nescala = (this->Nescala) + (this->_esca);
	this->_mod.setScale(glm::vec3(this->Nescala));

	this->rotation += 1;
	if (this->rotation > 360)
		this->rotation = 0;

	_mod.setRotation(glm::vec3(0.0, this->rotation, 0.0), 0.0f);

	if (this->Posi < 0 || this->Posi >1) {
		this->Posi = this->Posi > 1 ? 1 : 0;
		this->velPosi *= -1;
	}
	this->Posi = (this->Posi) + (this->velPosi);
	_mod.setPosition(glm::vec3(this->posiBase.x, this->Posi, this->posiBase.z));

}