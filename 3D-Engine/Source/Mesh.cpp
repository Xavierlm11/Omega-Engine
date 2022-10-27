#include "Mesh.h"

MeshData::MeshData()
{
	id_indices = 0; // index in VRAM
	num_indices = 0;
	indices = nullptr;

	id_vertices = 0; // unique vertex in VRAM
	num_vertices = 0;
	vertices = nullptr;
	hasLoadedBuffers = false;
}

MeshData::~MeshData()
{
	indices = nullptr;
	delete indices;

	vertices = nullptr;
	delete vertices;

	textures = nullptr;
	delete textures;
}


void MeshData::LoadBuffers() {

	if (hasLoadedBuffers == false) {
		glGenBuffers(1, (GLuint*)&(id_vertices));
		glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, &vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, (GLuint*)&(id_indices));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_indices, &indices[0], GL_STATIC_DRAW);

		glGenBuffers(1, (GLuint*)&(id_textures));
		glBindBuffer(GL_ARRAY_BUFFER, id_textures);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_textures * 2, &textures[0], GL_STATIC_DRAW);

		LOG("Mesh Loaded! Mesh indices: %i. Mesh vertices: %i", num_indices, num_vertices);
		hasLoadedBuffers = true;
	}
}

void MeshData::UnloadMesh() {

	if (hasLoadedBuffers == true) {

		if (id_indices != NULL) {
			glDeleteBuffers(1, &id_indices);
		}

		if (id_vertices != NULL) {
			glDeleteBuffers(1, &id_vertices);
		}

		if (id_textures != NULL) {
			glDeleteBuffers(1, &id_textures);
		}
		hasLoadedBuffers = false;
	}

	if (indices != nullptr) {
		delete[] indices;
	}

	if (vertices != nullptr) {
		delete[] vertices;
	}

	if (textures != nullptr) {
		delete[] textures;
	}

	indices = nullptr;
	vertices = nullptr;
	textures = nullptr;

}

void MeshData::DrawMesh(GLuint textureID) {
	if (hasLoadedBuffers == true) {

		if (textureID != NULL) {
			glBindTexture(GL_TEXTURE_2D, textureID);
		}


		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
		glVertexPointer(3, GL_FLOAT, 0, NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, id_textures);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);


		glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);

		if (textureID != NULL) {
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableClientState(GL_VERTEX_ARRAY);

		if (num_textures != 0) {
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}

		//LOG("Mesh Loaded! Num indices: %i. Num vertices: %i. ID Indices: %i. ID Vertices: %i.", num_indices, num_vertices, id_indices, id_vertices);

	}
}

