# Concept of Processes

1. **Set the CHILD_PATH environment variable:** Set the `CHILD_PATH` environment variable with the path to the child executable. For example: `/home/user/smth/other/child/child`.

2. **Compile the parent and child executables:** Compile the parent and child executables using the following commands:
    ```bash
    gcc -o parent parent.c parent_lib.c -W -Wall -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic
    gcc -o child child.c child_lib.c -W -Wall -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic
    ```

3. **Run the program:** Execute the parent program with the following command:
    ```bash
    ./parent <path/to/file/with/names/variables/environment>
    ```
    For example: 
    ```bash
    ./parent /home/user/smth/other/vars.txt
<<<<<<< HEAD
    ```
=======
    ```
>>>>>>> 82998a35313725c307415f27fbb728497cf94d67
