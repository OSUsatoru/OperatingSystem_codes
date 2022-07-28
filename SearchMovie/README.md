## Instruction
* Reads directory entries
* Finds a file in the current directory based on user specified criteria
* Reads and processes the data in the chosen file
* Creates a directory
* Creates new files in the newly created directory and writes processed data to these files

## Example of compile a code program to create an executable file
```sh
gcc --std=gnu99 -o movies_by_year main.c
```
## Example of run program commands
```sh
./movies_by_year
```
## sample output
```sh
1. Select file to process
2. Exit the program

Enter a choice 1 or 2: 1

Which file you want to process?
Enter 1 to pick the largest file
Enter 2 to pick the smallest file
Enter 3 to specify the name of a file

Enter a choice from 1 to 3: 1
Now processing the chosen file named movies_1.csv
Created directory with name chaudhrn.movies.89383

1. Select file to process
2. Exit the program

Enter a choice 1 or 2: 1

Which file you want to process?
Enter 1 to pick the largest file
Enter 2 to pick the smallest file
Enter 3 to specify the name of a file

Enter a choice from 1 to 3: 2
Now processing the chosen file named movies_2.csv
Created directory with name chaudhrn.movies.30886

1. Select file to process
2. Exit the program

Enter a choice 1 or 2: 1

Which file you want to process?
Enter 1 to pick the largest file
Enter 2 to pick the smallest file
Enter 3 to specify the name of a file

Enter a choice from 1 to 3: 3
Enter the complete file name: foo_bar
The file foo_bar was not found. Try again

Which file you want to process?
Enter 1 to pick the largest file
Enter 2 to pick the smallest file
Enter 3 to specify the name of a file

Enter a choice from 1 to 3: 3
Enter the complete file name: great_movies.csv
Now processing the chosen file named great_movies.csv
Created directory with name chaudhrn.movies.92777

1. Select file to process
2. Exit the program

Enter a choice 1 or 2: 2

```