# MATRIX PAGE STRUCTURE

Let Matrix A (NxN) divided into multiple smaller matrices a<sub>ij</sub> (nxn)

- DIMENSION(a<sub>ij</sub>)(n) = sqrt(BLOCK_SIZE*1024/4) //block_size*1024/4 would be the number of integers possible in a page.
- number of pages = ceil(N/n)^2
- max. size of a page= BLOCK_SIZE*1024/4*4(for each integer) = BLOCK_SIZE = 8KB (for this case)
						
						

A
||||||
|------|------|------|------|---|
| a<sub>11</sub> | a<sub>12</sub> | a<sub>13</sub> | a<sub>14</sub> |   |        
| a<sub>21</sub> | a<sub>22</sub> | a<sub>23</sub> | a<sub>24</sub> |   |
| a<sub>31</sub> | a<sub>32</sub> | a<sub>33</sub> | a<sub>34</sub> |   |
| a<sub>41</sub> | a<sub>42</sub> | a<sub>43</sub> | a<sub>44</sub> |   |


A<sup>T</sup>
||||||
|------|------|------|------|---|
| a<sup>T</sup><sub>11</sub> | a<sup>T</sup><sub>21</sub> | a<sup>T</sup><sub>31</sub> | a<sup>T</sup><sub>41</sub> |   |
| a<sup>T</sup><sub>12</sub> | a<sup>T</sup><sub>22</sub> | a<sup>T</sup><sub>32</sub> | a<sup>T</sup><sub>42</sub> |   |
| a<sup>T</sup><sub>13</sub> | a<sup>T</sup><sub>23</sub> | a<sup>T</sup><sub>33</sub> | a<sup>T</sup><sub>43</sub> |   |
| a<sup>T</sup><sub>14</sub> | a<sup>T</sup><sub>24</sub> | a<sup>T</sup><sub>34</sub> | a<sup>T</sup><sub>44</sub> |   |

- Number of datablocks access = no. of datablocks = **ceil(N/BLOCK_SIZE)^2**
- All submatrices need not be square but since input is a square and dim(a<sub>ij</sub>) = dim(a<sup>T</sup><sub>ji</sub>), it won't matter.

## constraints
- size of data_block <= KB
- Each integer is 4B.
- N <= 10^9

	