# Parallelizing-Mandelbrot-Set

Anas Albaqeri 


---

# Introduction

### Mandelbrot Set

The Mandelbrot set is a two-dimensional set with a relatively simple definition that exhibits great complexity, especially as it is magnified. It is popular for its aesthetic appeal and fractal structures. (Wiki)

The Mandelbrot set is represented using the following formula 

$z(n+1) = z_n^2 + c$    with   $z_0 = 0$

The Mandelbrot set bounds the magnitude of Zn to a certain limit and if that limit is exceeded for a number c, it is then said that c is not a member of the Mandelbrot Set 

### Mandelbrot and Parallelization

we can parallelize the computation of Mandelbrot set by dividing the task (in our case the image) into smaller pieces and allocating each to a processor or a thread to be worked on simultaneously. Each processor works individually on part of the image, preforming the same computation for each pixel within the allocated section determining whether each pixel is inside or outside the the set. 

### Setup

for this process, I used Kali Linux emulator to run the MPI program for Mandelbrot Set. The program was written and is copyrighted by Martin Ohman, 2015, and is free as stated by the author:

[https://github.com/martinohmann/mpi-mandelbrot/blob/master/mandelbrot.h](https://github.com/martinohmann/mpi-mandelbrot/blob/master/mandelbrot.h)

MPI version used: 

<img width="319" alt="Screenshot_2023-10-08_211905" src="https://github.com/Anas-Albaqeri/Parallelizing-Mandelbrot-Set/assets/127996785/68b2fb5b-d62f-49ce-9967-3e2b6fb09fbe">


Kali Specifications: 

<img width="545" alt="Untitled" src="https://github.com/Anas-Albaqeri/Parallelizing-Mandelbrot-Set/assets/127996785/5f4eeec2-4d99-4e67-92eb-7db47985ff1b">

for this task, I used 4 cores as 4 parallel processors working with 4 gb of RAM. This was the maximum my machine was able to handle for the Kali Emulator. 

---

## Sequential:

The sequential Mandelbrot solution is achieved by giving one processor the entire Image at once with all the processes happening in sequence. 

Here is a break down of the code used with explanation of each step: 

<img width="735" alt="Screenshot_2023-10-09_143926" src="https://github.com/Anas-Albaqeri/Parallelizing-Mandelbrot-Set/assets/127996785/e6147f80-8107-4d6c-a579-ff931857644a">


<img width="737" alt="Untitled 1" src="https://github.com/Anas-Albaqeri/Parallelizing-Mandelbrot-Set/assets/127996785/90091fd8-374a-445c-ab01-8a40e864287f">

1. **Include Necessary Tools**: The code first makes sure it can use some helpful tools, like the ability to do complex math, measure time, and handle input/output.
2. **Declare Variables**: It sets up some important values:
    - **`width`** and **`height`** determine the size of the picture (666x666 pixels).
    - **`x_min`**, **`x_max`**, **`y_min`**, and **`y_max`** define the area of the picture to look at.
    - **`iter_limit`** comes from a command you give when running the program and controls how detailed the picture will be.
    - **`data`** is a big list that will hold the results for each pixel.
    - **`start_time`** is like a stopwatch; it remembers when we start.
3. **Calculate the Mandelbrot Set**: It runs a function called **`compute_mandelbrot_subset`** to do the real work. We'll explain this function more later.
4. **Stop the Stopwatch**: It notes the time again now that the work is done (**`end_time`**).
5. **Calculate Time Taken**: It figures out how much time was spent doing the calculations.
6. **Print Results**: It prints the time taken and the iteration limit you gave.
7. **`compute_mandelbrot_subset` Function**:
    - This part of the code calculates a piece of the Mandelbrot set.
    - It needs a few things to work:
        - **`result`** is where it will store the calculated values.
        - **`iter_limit`** tells it how many times to repeat a math process.
        - **`x_resolution`** and **`y_resolution`** say how many pixels wide and tall the piece is.
        - **`x_begin`**, **`y_begin`**, **`x_end`**, and **`y_end`** mark the boundaries of the piece.
8. **Calculate Pixel Steps**: It calculates how much to move from one pixel to the next in the x and y directions (**`x_step`** and **`y_step`**).
9. **Loop Over Pixels**: It starts looking at each pixel one by one:
    - For each pixel, it figures out a complex number **`c`** based on where the pixel is.
    - It sets another complex number **`z`** to zero.
10. **Calculate Mandelbrot Value**: For each pixel, it calculates how long it takes for **`z`** to get big (or if it ever does):

I ran the test fifteen times sequentially, increasing the iterations by 200 each time and these are the results obtained each time:

also since this is a sequential code, I ran the code using the gcc compiler as it is best fit for the the sequential code and would give more accurate speedup factors later on when measuring. 

### Output

<img width="328" alt="Untitled 2" src="https://github.com/Anas-Albaqeri/Parallelizing-Mandelbrot-Set/assets/127996785/99cc9f10-b793-4858-ae47-469d43a9e665">
<img width="379" alt="Untitled 4" src="https://github.com/Anas-Albaqeri/Parallelizing-Mandelbrot-Set/assets/127996785/3f6fa1f7-4a40-4de2-b3b5-0ea8036af7e4">
<img width="474" alt="Untitled 5" src="https://github.com/Anas-Albaqeri/Parallelizing-Mandelbrot-Set/assets/127996785/a618ea35-dc29-4aa5-83b3-dee4c97cfd0c">
<img width="331" alt="Untitled 6" src="https://github.com/Anas-Albaqeri/Parallelizing-Mandelbrot-Set/assets/127996785/b81da5d1-ffd6-45b6-97e1-4f0447339670">


we notice that the computation time is increasing almost linearly with a bit of fluctuations as the number of iterations is increasing. 

 

---

## Dynamic Code

The code used runs MPI to Parallelize the computation across 4 processors. Master process here coordinates the computation and collect results from the “slave” processes while each perform part of the computation. Here is a break down of the dynamic aspects of the code: 

1. **Dynamic Workload Allocation**:
    - In the master process (**`master_proc`** function), rows of pixels are assigned to slave processes dynamically within a loop.
    - The loop sends rows to slave processes for computation using **`MPI_Send`**.
    - The master process keeps track of the number of running tasks and sends more work as needed.
    - This dynamic distribution of rows ensures that each slave process receives work continuously without waiting for all others to finish.
    
    ```c
    cCopy code
    while (running_tasks > 0) {
        // ...
        // Dynamically assign rows to slave processes.
        // ...
    }
    
    ```
    
2. **Adaptive Load Balancing**:
    - Adaptive load balancing is achieved by assigning rows to slave processes as soon as they become available.
    - The code ensures that slave processes don't remain idle by assigning more rows as they request additional work.
    - Load balancing is maintained throughout the computation.
3. **Progressive Row Processing**:
    - Rows of pixels are processed incrementally, and as each slave process completes its assigned work, it requests the next block of rows.
    - The master process responds to these requests by assigning more rows dynamically, allowing for continuous progress.
    
    ```c
    cCopy code
    while ((MPI_Recv(rows, opts->blocksize, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status) == MPI_SUCCESS) && status.MPI_TAG == MO_CALC) {
        // ...
        // Process rows and request more work dynamically.
        // ...
    }
    
    ```
    
4. **Communication Overheads**:
    - Communication overhead is introduced by the use of MPI communication functions (**`MPI_Send`** and **`MPI_Recv`**) to exchange data and coordinate work distribution.
    - The code measures communication time (**`comm_time`**) to evaluate the impact of these overheads.
5. **Progress Tracking**:
    - The code includes an option to print a progress bar to track the progress of the computation.
    - The progress bar dynamically updates as rows are processed, providing feedback on the ongoing computation.
    
    ```c
    cCopy code
    if (opts->show_progress) {
        // ...
        // Print a dynamic progress bar.
        // ...
    }
    
    ```
    

These dynamic aspects collectively enable efficient parallelization of the Mandelbrot set computation by ensuring that work is distributed dynamically among processes, load is balanced, and progress is continuously tracked.

### Output

Same 15 tests with iterations ranging from (1000 to 4000) were run using 2, 3 and 4 processors as follows:

## Using 4 Parallel processors

<img width="321" alt="Untitled 7" src="https://github.com/Anas-Albaqeri/Parallelizing-Mandelbrot-Set/assets/127996785/e1a199b9-ec46-4a32-a22b-cc53b97b16f4">

Image Generated at 1000 iterations 
![mandelbrot](https://github.com/Anas-Albaqeri/Parallelizing-Mandelbrot-Set/assets/127996785/b97cd716-d383-40cc-8e3b-d914f24d871a)


We repeat the test 14 times but with a cap of 2000 iterations as recommended by the code author: 

and we get the following results:

![Untitled 8](https://github.com/Anas-Albaqeri/Parallelizing-Mandelbrot-Set/assets/127996785/a96b5ae4-3158-4379-a858-51c7cead87aa)

Now repeating the same test on 3 parallelized processors:

<img width="481" alt="Untitled 9" src="https://github.com/Anas-Albaqeri/Parallelizing-Mandelbrot-Set/assets/127996785/88626643-128e-4cd1-867b-1fb471e13874">

Repeating the test up to 2000 iterations we get the following results: 

![Untitled 10](https://github.com/Anas-Albaqeri/Parallelizing-Mandelbrot-Set/assets/127996785/8d633e18-b07c-4b0e-98e4-89b9e860e8d0)

finally with two parallelized processors: 

<img width="403" alt="Untitled 11" src="https://github.com/Anas-Albaqeri/Parallelizing-Mandelbrot-Set/assets/127996785/c872e8e8-537a-4105-ac2e-751b19c6d0c1">

![Untitled 12](https://github.com/Anas-Albaqeri/Parallelizing-Mandelbrot-Set/assets/127996785/19770d40-bffa-4ffa-8127-e8dc4a49e1b8)

---

---

## Static

This code demonstrates a static parallelization approach for computing the Mandelbrot set, where each MPI process is responsible for a fixed portion of the grid, and the workload is evenly divided among processes. The results are gathered at the end for image generation. Timing measurements provide insights into the performance of the code

Here is a detailed break down of the used static code: 

1. **Maximum Iterations and Grid Size**:
    - **`#define MAXITER 1000`**: This macro defines the maximum number of iterations for the Mandelbrot set computation.
    - **`#define N 666`**: This macro defines the size of the grid, which determines the resolution of the Mandelbrot set image.
2. **Variable Declarations**:
    - **`int i, j, k, loop, groupSize, rank, ncpu, pos;`**: These variables are declared to store various indices and values used in the computation.
    - **`float *x, *y;`**: These arrays are declared to store computed values (**`x`**) and the final Mandelbrot set image data (**`y`**).
    - **`float complex z, kappa;`**: These complex numbers are used in the Mandelbrot set computation.
    - **`FILE *fp; int green, blue;`**: These variables are used for file handling and color calculations.
3. **MPI Initialization**:
    - **`MPI_Init(&argc, &argv);`**: MPI (Message Passing Interface) is initialized to enable parallel computing.
    - **`MPI_Comm_rank(MPI_COMM_WORLD, &rank); MPI_Comm_size(MPI_COMM_WORLD, &ncpu);`**: The rank and size of the MPI communicator are determined to identify each process and the total number of processes.
4. **Workload Division**:
    - **`groupSize = N * N / ncpu;`**: The total workload is divided equally among MPI processes based on the grid size (**`N`**) and the number of CPU cores (**`ncpu`**).
5. **Memory Allocation**:
    - **`x = (float *)malloc(groupSize * sizeof(float));`**: Memory is allocated dynamically for the **`x`** array, which stores computed values.
    - **`if (rank == 0) { y = (float *)malloc(N * N * sizeof(float)); }`**: Memory is allocated for the **`y`** array to store the final Mandelbrot set image data. This allocation is done only by the rank 0 process.
6. **Mandelbrot Set Computation**:
    - The Mandelbrot set computation is performed within a loop, and each MPI process computes a portion of the Mandelbrot set based on its rank and **`groupSize`**.
    - The results are stored in the **`x`** array.
7. **Timing Measurements**:
    - **`MPI_Wtime()`** is used to measure the time taken for various parts of the computation, including the Mandelbrot set calculation, waiting time, and communication time.
    - These time measurements are used to profile the performance of the code.
8. **Data Gathering**:
    - **`MPI_Gather(x, groupSize, MPI_FLOAT, y, groupSize, MPI_FLOAT, 0, MPI_COMM_WORLD);`**: The computed values (**`x`**) from all MPI processes are gathered to the rank 0 process (**`0`**) and stored in the **`y`** array.
9. **Printing Timings and Finalization**:
    - Timings for calculation, waiting, and communication are printed for each MPI process.
    - The rank 0 process writes the Mandelbrot set image data to a file named "mandelbrot1.ppm" based on the computed values in the **`y`** array.
    - Memory allocated for arrays is released, and MPI is finalized.
    

Now for our testing, we manually redefine the max iterations possible for run:

<img width="335" alt="Untitled 13" src="https://github.com/Anas-Albaqeri/Parallelizing-Mandelbrot-Set/assets/127996785/d6658bfd-7330-4b97-bcaf-0a62287ae418">

The results obtained shows a clear example of load imbalance between the processors as processor 1 is taking almost 4 times the time processor 2 is taking:

<img width="180" alt="Untitled 14" src="https://github.com/Anas-Albaqeri/Parallelizing-Mandelbrot-Set/assets/127996785/850fab31-3f08-49c1-ba6e-17415217fac7">

The image generated with 2 parallelized processors :

<img width="335" alt="Untitled 15" src="https://github.com/Anas-Albaqeri/Parallelizing-Mandelbrot-Set/assets/127996785/c58e028c-be23-439a-bba9-fb8535e467ff">

### 3 parallel processors - static

<img width="160" alt="Untitled 16" src="https://github.com/Anas-Albaqeri/Parallelizing-Mandelbrot-Set/assets/127996785/44affceb-fe40-4cc2-bd2d-86bbb17bc234">

### 4 parallel processors - static

<img width="213" alt="Untitled 17" src="https://github.com/Anas-Albaqeri/Parallelizing-Mandelbrot-Set/assets/127996785/e1c3ae8a-2ebf-48b4-8c5c-b20515e4450c">

we notice from the previous tests that there is an apparent improvement in the computation time. However, we still have a huge overhead in communication in oppose to the dynamic output. 

---

# Speed up Factor:

In order to get the most reliable speed up factor in our case; we fix the number of iterations at 1000 for all tests and compare computation time with best sequential time acquired. 

for that purpose: we have 6 different speedup factors for the test cases used: 

1- Using 2 processors dynamically 

<img width="357" alt="Untitled 18" src="https://github.com/Anas-Albaqeri/Parallelizing-Mandelbrot-Set/assets/127996785/7cb431f3-a303-4845-888e-80642166277f">

referring back to the formula we get the following results 

$S(p) = 2.108884/ 1.02635$  = 2.05474

2-  Using 3 processors dynamically 

$S(p) = 2.108884/ 0.597193$ = 3.531327393

3- Using 4 processor dynamically 

S(p) = 2.108884/ 0.363935 = 5.794672 

4- using 2 processors statically 

$S(p) = 2.108884/ 5.116615$ = 0.41126 

5- Using 3 processors statically 

$S(p) = 2.108884/ 2.903823$ = 0.72624

6- Using 4 processors statically 

$S(p) = 2.108884/ 2.491282$ = 0.81

### Discussion

As we saw earlier, the computation of Mandelbrot Set using dynamic and static approaches gave varying degrees of speedup depending on the method used and number of parallel processors. Dynamically, utilizing 3 and 4 processors in parallel gave excellent speedup factors of around 3.53 and 3.8 respectively, which suggest efficient parallelism and almost linear scaling. However, this result might indicate that sequential processing suffered lots of overhead and underutilization of resources, especially that this was run on a virtual machine with limited resources. On the other hand, the static approach with 2,3,4 processors resulted in worse times than the sequential one. This indicate issues such as load imbalance and communication overhead. It is needed to mention in this case that this issue might have caused mostly due to the virtual machine being scheduled by the CPU as it tries to execute simultaneous tasks. Notably, the dynamic approach demonstrated better scalability and more promising speedup, highlighting the significance of load balancing techniques and data distribution optimizations in achieving efficient parallel performance.

### References

---

Please note that I have used these following tools to set up conduct the experiment, analysis and troubleshooting issues for this Assignment

1. ChatGPT.
2. Wikipedia.
3. Vinod Kumar Rajasekaran. "Impact of Virtual Machine Overhead on TCP Performance." Submitted to the graduate faculty of the Department of Computer Science, Clemson University, June 17, 2004.
4. Martin Ohmann. "Dynamic MPI Mandelbrot Algorithm." Copyright (C) 2015. This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation.

 

---
