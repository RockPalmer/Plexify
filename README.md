# Plexify
This program is meant to help software developers to simplify the logic within their programs. This version is built for windows devices.
To run the program, download the `bin` folder and run `bin\plexify.exe`.

Upon running the program, you will be given the choice of 7 commands that you can run from this terminal.

### `generate`

This command generates an empty truth table for the user (i.e. where all outputs are set to `0`). This user will give the input variables
for the table, the output variables for the table, and the name the user wants to give to the generated file. The program will create a
CSV file that contains a truth table fitting these parameters. From here, the user can load this document into Microsoft Excel or any other
text editor to add the truth values for the inputs they want.

### `load`

This command loads a truth table file from a local CSV file. An example truth table file can be found in the `bin` folder named `Test_cases.csv`.

### `print`

This command prints the currently loaded truth table to the console. If there is no truth table loaded into memory, the program will ask the
user to please load a truth table.

### `sop` and `pos`

These to commands generates and loads into memory a minimum sum of products or product of sums expression respectively for a variable the user
provides.

### `simplify`

Once a boolean expression is loaded into memory, this command, using DeMorgan's laws, tries to simplify the expression as much as possible by
reducing the number of "NOT" operations in the expression. If there is no expression loaded into memory, the program will ask the user
to please load one.

### `help`

Displays the list of commands.

### `quit`

Ends the program.

# Examples

The file `Test_cases.csv` has been included in this repository to demonstrate the capabilities of this program. Once the user has loaded
this truth table into memory, they can get expressions for the various different output variables to demonstrate the program.

* Output `d` demonstrates a basic boolean expression the user might want to solve for.
* Output `e` demonstrates specifically, an expression that would be more succinctly as a sum of products.
* Output `f` demonstrates specifically, an expression that would be more succinctly as a product of sums.
* Output `g` demonstrates a boolean expression that includes "don't care" cases.
* Output `h` demonstrates an output that is always off.
* Output `j` demonstrates an output that is always on.
