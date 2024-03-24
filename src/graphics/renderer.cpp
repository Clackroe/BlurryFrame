#include <graphics/renderer.hpp>

Renderer::Renderer(Camera* cam)
{
    camera = cam;
}

Renderer::~Renderer() { }

void Renderer::renderImage(Image& image)
{
    // Camera Setup
    shader->use();
    shader->setMat4("proj", camera->getProjection());
    shader->setMat4("view", camera->getView());

    shader->setInt("image", image.textureSlot);
    shader->setMat4("model", image.getModelMatrix());

    glActiveTexture(GL_TEXTURE0 + image.textureSlot);
    glBindTexture(GL_TEXTURE_2D, image.texID);
    glBindVertexArray(image.VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
