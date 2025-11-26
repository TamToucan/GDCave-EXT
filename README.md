# Procedural Cave Generation

Provides a variety of means to generate a 2D "cave" of given FLOOR/WALL values.

# Examples

This document outlines the configuration presets for the procedural cave generation system. The code below shows the required setup followed by the various `wall_chance` settings and generation algorithms available.

## Base Setup
*This initialization is required before running any of the examples below.*

```gdscript
# Seed, Size, Border, CellSize, Octaves, Wall Atlas
caveData = setup(424242, Vector2i(Utils.g_maze_width,Utils.g_maze_height),
        Vector2i(8,8), Vector2i(8,8), 1, FLOOR, WALL)
caveData.noise_freq = 13.7
caveData.use_perlin = false
```

---

## Generation Configuration Examples

Apply one of the following blocks to achieve the desired map style.

### 1. Organic & Open
*Standard organic shapes.*

```gdscript
caveData.wall_chance = 0.50
caveData.add_gen_3x3(5,8, 4,8, 6)  # curvy, open
caveData.add_gen_3x3(6,8, 3,8, 6)  # less curvy
caveData.add_gen_3x3(4,4, 4,8, 5)  # open with islands
```

### 2. Balanced / Semi-Open
*Slightly more open, with gap filling.*

```gdscript
caveData.wall_chance = 0.40
caveData.add_gen_3x3(4,5, 4,7, 4)       # 50/50 caves
caveData.add_gen_3x3(3,8, 1,8, 1)       # fill in orz/very single gaps (makes it curvy?)
```

### 3. Sparse Single-Track Maze
*Creates single-track maze bits with dead ends.*

```gdscript
caveData.wall_chance = 0.20
caveData.add_gen_3x3(3,3, 1,5, 10) # Grows single track maze bits
```

### 4. Connected Single-Track Maze
*Similar to sparse maze, but with fewer dead ends.*

```gdscript
caveData.wall_chance = 0.40
caveData.add_gen_3x3(3,3, 2,4, 10) # Grows single track, but less dead
```

### 5. Open Maze
*A more open variation of the maze structure.*

```gdscript
caveData.wall_chance = 0.35
caveData.add_gen_3x3(3,3, 2,4, 6)       # another maze, open
```

### 6. Complex Curvy Caves (5x5)
*Uses the 5x5 algorithm for smoothing large open areas.*

```gdscript
caveData.wall_chance = 0.40
caveData.add_gen_3x3_5x5(5,9, 15,25, 3,8, 15,20, 4)     # open curvy caves
```

### 7. High Density (Swiss Cheese)
*Creates solid walls with holes. Note: Wall chance is high (0.65).*

```gdscript
caveData.wall_chance = 0.65 # Can't really go below 40, too many walls
caveData.add_gen_3x3_5x5(3,4, 12,16, 2,5, 10,14, 2) # open with areas with holes in middle

# Optional cleanup:
#caveData.add_gen_3x3(9,9, 2,9, 2) # remove singles
#caveData.add_gen_3x3(3,8, 1,8, 1)      # fill in (most) of the holes
```

### 8. Broken Wall Areas
*Creates open areas containing broken wall segments.*

```gdscript
caveData.wall_chance = 0.40 # open areas, 50 = walls with room/gaps
caveData.add_gen_3x3_5x5(4,5, 13,17, 4,5, 14,20, 4) # "broken" wall areas

# Optional cleanup:
#caveData.add_gen_3x3(3,8, 1,8, 1)      # fill in (most) of the holes/breaks
```
