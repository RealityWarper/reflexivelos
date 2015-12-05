# Dumb algorithm #

The algorithm is simple: generate each digital line in turn, and walk along each one until you hit an obstruction. Here it is in psuedocode:

```
for 0 <= p <= q <= n and 0 <= s < q
  offset = s, y = 0
  for x = 1 to n
    offset = offset + p
    if (offset >= q)
      offset = offset - q
      y = y + 1

    mark_visible(x, y)

    if (obstruction(x, y))
      leave inner loop
```

Obviously, this is extremely slow - running time is O(N<sup>4</sup>).

# A little bit smarter #

Notice that if we fix p and q, there are only two possible heights for the line at any particular x coordinate. If both of those squares are blocked, no value of s will work, and if both are empty, they don't impose any extra conditions on s. If the bottom one is blocked, then smaller values of s will be excluded, and similarly for the top. So at each x coordinate, there will be an interval of possible values for s, which can be maintained easily. Here is some psuedocode:

```
for 0 <= p <= q <= n
  lower_s = 0, upper_s = q-1
  lower_offset = lower_s, upper_offset = upper_s
  lower_y = 0, upper_y = 0

  for x = 1 to n    
    lower_offset = lower_offset + p
    if (lower_offset >= q)
      lower_offset = lower_offset - q
      lower_y = lower_y + 1

    mark_visible(x, lower_y)

    if (obstruction(x, lower_y))
      lower_s = lower_s + q - lower_offset
      lower_offset = 0
      lower_y = lower_y + 1

    # similarly for upper

    if (lower_s > upper_s)
      leave inner loop
```

This does a little better - running time is O(N<sup>3</sup>). But best possible is O(N<sup>2</sup>)...

# Between two points #

Let's take a break from field of view algorithms... with a line of sight algorithm!

At this point, we need a little bit of geometric intuition. As far as digital lines in the first octant are concerned, all of the action happens at integral x coordinates - each obstruction can be thought of as a horizontal line segment of length one, as can the source and destination. We want to know whether there is a line connecting the source line segment to the destination line segment that doesn't cross any of the obstructing segments. Thus, we can restrict attention to the parallelogram formed by the source and destination segments. Just like the previous algorithm, for each value of x there are only two relevant segments intersecting the parallelogram, each of which may or may not be obstructing.

Thus, obstructions can be classified as upper obstructions and lower obstructions, depending on which edge of the parallelogram they intersect. We want to know  whether there is a line going underneath all of the upper obstructions and above all of the lower obstructions, that is, whether the convex hull of the upper obstructions intersects the convex hull of the lower obstructions. This is easy to check, though - we can find the convex hulls of each set in O(N) time, and then check this condition at every integral x coordinate from 1 to n.

# A Ray Casting idea #

In the previous algorithm, we could have been a little bit more ambitious. Instead of simply checking if we can see the destination, why don't we check if we can see the intermediate segments (or at least, their intersections with the parallelogram we're restricting ourselves to)?

If we can do this, then we can solve the field of view problem in O(N<sup>2</sup>) time! We simply call the previous algorithm with destinations (n, 0), (n, 1), ..., (n, n). This works because the parallelograms formed from the source and each destination completely cover the first octant, so if an intermediate segment is visible, a visible part of it will be contained in one particular parallelogram, and the segment will be marked visible while running the previous algorithm with the corresponding destination.