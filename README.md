# A Basic Graphics Engine

## Language Syntax

### General

Indentation doesn't matter but makes things easier

### Evaluated Expressions

Anything inside `()` will be evaluated

| Expression                     | Description                                           | Example    | Returns         |
|--------------------------------|-------------------------------------------------------|------------|-----------------|
| +, -, *, /, %                  | Should be fairly obvious                              | (5 / 2)    | double          |
| <, <=, >, >=, ==, !=, &&, \|\| | Also pretty standard                                  | (1 == 2)   | double (1 or 0) |
| rand                           | Returns a random int in range                         | (rand 100) | int             |
| sin, cos                       | Returns the expected value, input should be in degres | (sin 60)   | double          |

### Variables

Can be set with `set <variable> <value>` or in for loops(see below)

Values stored as strings in table

When you want to get the variable value, prefix the variable name with `$`: `x` => `$x`

Variables are scoped. If a variable is first set locally when the code block finishes executing it will be deleted. If the variable was defined in a higher scope then the value of the variable in the higher scope will be changed.

Scopes include functions, for loops, if statements, etc...

### Functions

Syntax: `function <function_name> <arg1> <arg2> ...`.

The function body should be followed with the `end_function` command.

Arguments are pass by value.

The function can be called with `<function_name> <arg1> <arg2> ...`. Example: `my_function 1 2 3`.

The script will execute the function `main` automatically, be sure to define it.

### For Loops

| Syntax                                      | Behavior                                                                              |
|---------------------------------------------|---------------------------------------------------------------------------------------|
| `for <variable> <start> <stop> <increment>` | Loop over and update `<variable>` from `<start>` to `<stop>` in increments of `<inc>` |
| `for <variable> <start> <stop>`             | Assume `<increment>` is 1                                                             |
| `for <variable> <stop>`                     | Assume `<start>` is 0 and `<increment>` is 1                                          |

`<variable>` is deleted upon exiting the loop, as are all variables defined within
The for loop body should be followed with the `end_for` command

### If/Else Statements
```
if (statement)
	do something
else
	do something else
end_if
```
Fairly self explanatory

### General Commands
| Command    | Syntax                                                                              | Description                                                                                                                                                                                                    |
|:-----------|:------------------------------------------------------------------------------------|:---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Display    | `display <filename>`                                                                | Displays current canvas, or `<filename>` if specified.                                                                                                                                                         |
| Clear      | `clear`                                                                             | Clears the current contents of the canvas.                                                                                                                                                                     |
| Save       | `save <filename> <prefix>`                                                          | Saves the canvas to `<filename>`, if prefix is given the numerical prefix is added to the front. Useful for animations.                                                                                        |
| Gif        | `gif <basename> <filename>`                                                         | Collects all the images with `<basename>` and saves them to a gif stored in `<filename>`.                                                                                                                      |
| Dimensions | `dimensions <width> <height>`                                                       | Sets the canvas size in pixels.                                                                                                                                                                                |
| Push       | `push`                                                                              | Adds to the transformation stack.                                                                                                                                                                              |
| Pop        | `pop`                                                                               | Pops from the transformation stack.                                                                                                                                                                            |
| Srand      | `srand <seed>`                                                                      | Seeds the random number generator with `<seed>` if given, else the time.                                                                                                                                       |
| Color      | `color <red> <green> <blue>`                                                        | Sets the RGB color of the object. If `<red>` is -2, each triangle will have a random color, if -1, the objects will be displayed as wireframes.                                                                |
| Texture    | `texture <ambient> <diffuse> <specular>`                                            | Sets the texture of an object.                                                                                                                                                                                 |
| Light      | `light <name> <x> <y> <z> <red> <green> <blue>`                                     | Adds a light named `<name>` to the canvas positioned at `(<x>, <y>, <z>)` and with the given RGB value. If the RGB value isn't present, its assumed to be (255, 255, 255).                                     |
| Line       | `line <x1> <y1> <z1> <x2> <y2> <z2>`                                                | Draws a line between two points.                                                                                                                                                                               |
| Bezier     | `bezier <x1> <y1> <x2> <y2> <x3> <y3> <x4> <y4>`                                    | Creates a Bezier curve with the given values. The backend function can have arbitrary degrees and z values, but the parser has yet to reflect this.                                                            |
| Hermite    | `hermite <x1> <y1> <x2> <y2> <x3> <y3> <x4> <y4>`                                   | Creates a Hermite curve with the given values.                                                                                                                                                                 |
| Circle     | `circle <x> <y> <z> <radius>`                                                       | Creates a circle with the given values                                                                                                                                                                         |
| Box        | `box <x> <y> <z> <width> <height> <depth>`                                          | Assuming the top left hand corner is `(<x>, <y> <z>)`, creates a box with the given dimesnions                                                                                                                 |
| Sphere     | `sphere <x> <y> <z> <radius>`                                                       | Creates a sphere with the given values.                                                                                                                                                                        |
| Torus      | `torus <x> <y> <z> <inner_radius> <outer_radius>`                                   | Creates a torus with the given values.                                                                                                                                                                         |
| Cone       | `cone <x> <y> <z> <theta> <phi> <inner_angle> <radius> <resolution> <closed>`       | Generates a cone at the given point, angled a certain direction around the tip, either open or closed (0, 1).                                                                                                  |
| Tendril    | `tendril <x> <y> <z> <theta> <phi> <variance> <length> <radius> <end> <resolution>` | Generates a tendril starting at the given origin at the given angle. The tendril wiggles within the bounds of `<variance>` for `<length>` segments. The `<end>` can be Sphere(0), Cone(1), or Tapered_Cone(2). |
| Speckle    | `speckle <x> <y> <z> <width> <height> <depth> <density> <radius> <spiked>`          | Follows the same basic template as box, randomly fills it with speckles at a `<density>` chances. `<spike>`(0, 1) determines whether or not the edges are jagged.                                              |
| Flower     | `flower <x> <y> <z> <theta> <phi> <variance> <length> <tendrils> <bud>`             | Creates `<tendrils>` flowers growing from the given point. `<bud>`(0, 1) determines whether or not the flowers should have buds.                                                                               |
| Scale      | `scale <x> <y> <z>`                                                                 | Scales the elements by the given values.                                                                                                                                                                       |
| Move       | `move <x> <y> <z>`                                                                  | Tranlates the elements by the given values.                                                                                                                                                                    |
| Rotate     | `rotate <axis> <degrees>`                                                           | Rotates the elements about `<axis>`("x", "y", "z) by `<degrees>`                                                                                                                                               |

