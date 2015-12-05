# Introduction #

Have you ever played Zelda/Pokemon/etc. on a Game Boy Color? If you have, you probably understand what I mean when I say that it is nice to have houses which are bigger on the inside than the outside. Or maybe you've played Portal, and would like a game where you can create portals between two locations on the fly. Or maybe, instead of stairs taking you from one level to the next by using a special command (usually '>' or '<'), you'd rather have an actual staircase you can walk through using regular movement commands.

Well, the simplest way (conceptually) to do all of the above is to represent your map not as a grid of walls and empty spaces, but as a manifold. Unfortunately, the naive approach fails - how do you represent a manifold? How do you display it the same way you display a regular roguelike?

# Paste #

Manifolds can be constructed by pasting triangles together along the edges of the triangles. Of course, triangles are not well suited to being displayed on a grid, so instead we'll restrict to manifolds constructed by pasting squares together. To simplify life, to allow diagonal movement, and to allow field of vision to make sense, we'll also require that moving to the right and then up is the same as moving up and then to the right. We'll also complicate things by allowing ourselves to paste to squares together at the corners.

Now we have something which bears very little resemblance to manifolds. These are what I call grid-manifolds...

# One square at a time #

So we have these little squares, right? Let's look at just one of them:

```
1-----2
|     |
|  ^  |
|     |
3-----4
```

Because of the possible ways the squares can be joined together, it's possible that one could walk through the squares and wind up back where you started, except you'll be rotated:

```
3-----1
|     |
|  >  |
|     |
4-----2
```

Or maybe even reflected:

```
2-----1
|     |
|  ^  |
|     |
4-----3
```

To keep track of the info, we'll create a data structure for "positions":

```
struct position {
  int whichSquare, rotatedBy, mirrored;
};
```

Since squares don't really live in isolation, we also need to keep track of the connections between the squares. We need to store more information than which squares are connected to which squares, though: we need to be able to figure out whether we'll be rotated/mirrored in the new square!

In order to keep the actual code as simple as possible, I adopt the following convention for encoding directions as numbers: 0 is down, 1 is down+right, 2 is right, 3 is up+right, etc. (that is, numbers go around counterclockwise). This way, rotation is just addition mod 8, and reflection is just subtraction mod 8. So `rotatedBy` ought to be an even number from 0 to 6, and `mirrored` will be 1 or -1 (this way, multiplying by `mirrored` reflects us if we need to be reflected).

Now say we want to move in direction `dir`. Since we may be rotated/reflected, we first multiply `dir` by `mirrored`, and then add it to `rotatedBy`, to get the direction we'd move in if we weren't rotated/reflected. Then we change `whichSquare` based on this new direction, and update `rotatedBy` and `mirrored` using the information we stored earlier. Since this is so complicated, we'll encapsulate it into special functions:

```
void pos::reflect() {
  mirrored = -mirrored;
}

void pos::rotateBy(int rotation) {
  rotatedBy = (rotatedBy + rotation*mirrored)%8;
}

void pos::move(int dir) {
  int realDir = (dir*mirrored + rotatedBy)%8;
  if (connectionIsReflected[whichSquare][realDir]) {
    reflect();
    rotatedBy = (2*realDir - rotatedBy)%8;
  }
  rotateBy(connectionRotation[whichSquare][realDir] * mirrored);
  whichSquare = newSquare[whichSquare][realDir];
}
```

More info will come....