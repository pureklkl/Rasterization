#include "stdafx.h"
#include "Texture.h"


Texture::Texture(GzTexture texFun){
	this->texFun = texFun;
	texPlanar[U] = new TriPlanar();
	texPlanar[V] = new TriPlanar();
}


Texture::~Texture(){
	delete texPlanar[U];
	delete texPlanar[V];
}
