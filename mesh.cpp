#include <glad/glad.h>
#include <assimp/scene.h>

#include <iostream>
#include "shaders.h"
#include "geometry.h"


Mesh::Mesh(const aiMesh* m){
    vertexData.resize(m->mNumVertices);
    for (unsigned int i = 0; i < m->mNumVertices; i++){
        auto& pos = m->mVertices[i];
        vertexData[i].position = glm::vec3(pos.x,pos.y, pos.z);
        
        auto& n = m->mNormals[i];
        vertexData[i].normal = glm::vec3(n.x,n.y,n.z);
        
        if(m->HasTextureCoords(0)){
            auto& uv = m->mTextureCoords[0][i];
            vertexData[i].texCoord = glm::vec3(uv.x,uv.y, 0.f);
        }else{
            vertexData[i].texCoord = glm::vec3(0.f);
        }

        if(m->HasTangentsAndBitangents()){   
            auto& tan = m->mTangents[i];
            vertexData[i].tan = glm::vec3(tan.x,tan.y,tan.z);
        }

        if(m->HasVertexColors(0)){
            auto& col = m->mColors[0][i];
            vertexData[i].vCol = glm::vec4(col.r, col.g, col.b, col.a);
        }
    }

    for (unsigned int i=0; i < m->mNumFaces; i++){
        for(unsigned int j = 0; j < m->mFaces[i].mNumIndices; j++){
            indices.push_back(m->mFaces[i].mIndices[j]);
        }
    }
    AllocateBindBuffers();

    indexCount = indices.size();
    vertexData.clear();
    indices.clear();
}

void Mesh::Draw(Shader* s){
    glBindVertexArray(vertArrObj);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void Mesh::AllocateBindBuffers(){
    glGenVertexArrays(1, &vertArrObj);
    glGenBuffers(1, &vertBuffObj);
    glGenBuffers(1,  &elemBuffObj);

    glBindVertexArray(vertArrObj);
    glBindBuffer(GL_ARRAY_BUFFER, vertBuffObj);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(VertexData), &vertexData[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elemBuffObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*) (sizeof(float) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*) (sizeof(float) * 6));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*) (sizeof(float) * 8));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*) (sizeof(float) * 11));
    
    glBindVertexArray(0);
}