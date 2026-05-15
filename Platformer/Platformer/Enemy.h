#pragma once
#include <SFML/Graphics.hpp>
#include "Animation.h"
#include "World.h"

using namespace sf;

enum class EnemyType { Orc, Soldier };
enum class EnemyState { Patrol, Chase, Attack, Hit, Dead };

class Enemy {
public:
    EnemyType getType() const { return type; }

    static constexpr int   FRAME_W = 100;
    static constexpr int   FRAME_H = 100;
    static constexpr int   IDLE_FRAMES = 6;
    static constexpr int   WALK_FRAMES = 8;
    static constexpr int   ATK_FRAMES = 6;
    
    static constexpr float GRAVITY = 1800.f;
    static constexpr float MOVE_SPEED = 60.f;   // медленнее игрока (210)
    static constexpr float HITBOX_W = 28.f;
    static constexpr float HITBOX_H = 58.f;
    static constexpr float JUMP_SPEED = 520.f;
    static constexpr float DETECT_RADIUS = 350.f;  // меньше экрана
    static constexpr float ATTACK_RADIUS = 52.f;
    static constexpr float ATTACK_COOLDOWN = 1.4f;
    static constexpr float PATROL_TURN = 2.8f;
    static constexpr float HIT_DURATION = 0.35f;
    static constexpr float INVINCIBLE_TIME = 0.5f;// неуязвимость после удара
    static constexpr float SPRITE_SCALE = 2.5f;  // 100 * 2.5 = 250px
    static constexpr float SPRITE_SCALE_X = 1.6f;
    static constexpr float SPRITE_SCALE_Y = 3.2f;

private:

    EnemyType  type;
    EnemyState state = EnemyState::Patrol;
    float invincibleTimer = 0.f;

    bool blockedX = false;  // столкнулся со стеной по X в этом кадре

    Texture  tex;
    Sprite   sprite;
    Animator animator;
    AnimClip clipIdle, clipWalk, clipAttack;

    Vector2f pos;
    Vector2f vel;
    bool onGround = false;
    bool facingLeft = false;

    int   hp = 3;
    float attackTimer = 0.f;
    float patrolTimer = 0.f;
    float hitTimer = 0.f;
    float patrolDir = 1.f;

    // Флаги удара: hitting=true в момент кадра удара, сбрасывается через resetHit()
    bool hitting = false;

    void buildClips();
    void moveX(const World& world, float dx);
    void moveY(const World& world, float dy);
    void updateAI(float dt, Vector2f playerCenter);

public:
    Enemy(EnemyType type, float worldX, float worldY);
    bool loadTexture(const std::string& path);

    void update(float dt, const World& world, Vector2f playerCenter);
    void render(RenderWindow& window);

    void takeDamage(int dmg);

    // Возвращает true в момент кадра удара атаки
    bool isHitting() const { return hitting; }
    // Сбросить флаг удара после обработки (чтобы не бить дважды)
    void resetHit() { hitting = false; }

    bool isDead()  const { return hp <= 0; }
    bool isAlive() const { return hp > 0; }

    FloatRect getHitbox() const { return { pos.x, pos.y, HITBOX_W, HITBOX_H }; }
    Vector2f  getCenter() const { return pos + Vector2f(HITBOX_W / 2.f, HITBOX_H / 2.f); }
    int       getHp()     const { return hp; }
};