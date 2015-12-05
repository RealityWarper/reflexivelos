This is a roguelike engine I've been developing in my spare time. I'm putting it online so I don't lose it when my computer explodes, but please, reuse the code. I'm also putting some of the ideas in the wiki - however, you can probably find better versions of these articles on roguebasin (look for "digital field of view").

The selling point is an O(N<sup>2</sup>) symmetric field of view algorithm based on "digital lines". The neat thing is that the maps displayed don't even need to be grids, but can instead be weird manifold-y things.

I'm also currently working on random manifold generation, but that part of the code needs a little debugging (it works, but I don't know that).

To do list:
  * speed up best case run-time of field of view algorithm.
  * understand random generation of manifolds.
  * randomly generate manifolds as you explore them.
  * make this into an actual roguelike.
  * write up proof that maximum number of visible obstructions is O(N<sup>3/2</sup>).
  * randomly generate covers of given manifolds.

Note: all of the stuff in the downloads tab is stuff I put there before I learned about Subversion. I'm leaving it there for now because sometimes I get nostalgic...