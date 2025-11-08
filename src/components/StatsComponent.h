//
// Created by travis on 11/5/25.
//
#pragma once

#include <spdlog/spdlog.h>

struct StatsComponent {
    // Core stats
    int level = 1;
    int experience = 0;
    int experienceToNextLevel = 100;

    // Health
    int currentHP = 20;
    int maxHP = 20;

    // Magic
    int currentMP = 10;
    int maxMP = 10;

    // Attributes (Ultima-style)
    int strength = 10;      // Affects melee damage
    int dexterity = 10;     // Affects accuracy/dodge
    int intelligence = 10;  // Affects magic power

    // Resources
    int gold = 100;
    int food = 200;

    StatsComponent() = default;

    // Helper methods
    void TakeDamage(int damage) {
        currentHP -= damage;
        if (currentHP < 0) currentHP = 0;
    }

    void Heal(int amount) {
        currentHP += amount;
        if (currentHP > maxHP) currentHP = maxHP;
    }

    void SpendMP(int amount) {
        currentMP -= amount;
        if (currentMP < 0) currentMP = 0;
    }

    void RestoreMP(int amount) {
        currentMP += amount;
        if (currentMP > maxMP) currentMP = maxMP;
    }

    bool IsDead() const {
        return currentHP <= 0;
    }

    void GainExperience(int exp) {
        experience += exp;
        // Check for level up
        while (experience >= experienceToNextLevel) {
            LevelUp();
        }
    }

    void LevelUp() {
        level++;
        experience -= experienceToNextLevel;
        experienceToNextLevel = static_cast<int>(experienceToNextLevel * 1.5f);

        // Stat increases on level up
        maxHP += 5;
        currentHP = maxHP;
        maxMP += 3;
        currentMP = maxMP;
        strength++;
        dexterity++;
        intelligence++;

        spdlog::info("Level up! Now level {}", level);
    }
};