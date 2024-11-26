#include "AnimatedSprite.h"
#include "Texture2D.h"
#include <utility>
#include "iostream"

RenderEngine::AnimatedSprite::AnimatedSprite(
        const std::shared_ptr<Texture2D> &pTexture,
        const std::string &initialSubTextureName,
        const std::shared_ptr<ShaderProgram> &pShaderProgram,
        const glm::vec2 &position,
        const glm::vec2 &size,
        const float &rotation
) : Sprite(
        pTexture,
        initialSubTextureName,
        pShaderProgram,
        position,
        size,
        rotation
) {
    m_pCurrentAnimationDurations = m_statesMap.end();
}

void RenderEngine::AnimatedSprite::insertState(
        std::string state,
        std::vector<std::pair<std::string, uint64_t>> subTexturesDuration
) {
    m_statesMap.emplace(std::move(state), std::move(subTexturesDuration));
}

void RenderEngine::AnimatedSprite::render() const {
    if (m_hasChanges) {
        auto subTexture = m_pTexture->getSubTexture(
                m_pCurrentAnimationDurations->second[m_currentFrame].first
        );

        const GLfloat textureCoords[] = {
                // U  V
                subTexture.leftBottomUV.x, subTexture.leftBottomUV.y,
                subTexture.leftBottomUV.x, subTexture.rightTopUV.y,
                subTexture.rightTopUV.x, subTexture.rightTopUV.y,
                subTexture.rightTopUV.x, subTexture.leftBottomUV.y,
        };

        m_textureCoordsBuffer.update(textureCoords, 2 * 4 * sizeof(GLfloat));
        m_hasChanges = false;
    }

    Sprite::render();
}

void RenderEngine::AnimatedSprite::update(uint64_t delta) {
    if (m_pCurrentAnimationDurations != m_statesMap.end()) {
        m_currentAnimationTime += delta;

        while (m_currentAnimationTime >= m_pCurrentAnimationDurations->second[m_currentFrame].second) {
            m_currentAnimationTime -= m_pCurrentAnimationDurations->second[m_currentFrame].second;
            ++m_currentFrame;
            m_hasChanges = true;

            if (m_currentFrame == m_pCurrentAnimationDurations->second.size()) {
                m_currentFrame = 0;
            }
        }
    }
}

void RenderEngine::AnimatedSprite::setState(std::string newState) {
    auto it = m_statesMap.find(newState);
    if (it == m_statesMap.end()) {
        std::cout << "Can't find animation state: " << newState << std::endl;
        return;
    }

    if (it != m_pCurrentAnimationDurations) {
        m_currentAnimationTime = 0;
        m_currentFrame = 0;
        m_pCurrentAnimationDurations = it;
        m_hasChanges = true;
    }
}


