#include <iostream>
#include <fstream>
using namespace std;

struct Property {
	int ccLabel;
	int noOfPixels;
	int minRow;
	int minCol;
	int maxRow;
	int maxCol;
	
	Property() {
		ccLabel = 0;
		noOfPixels = 0;
		minRow = 0;
		minCol = 0;
		maxRow = 0;
		maxCol = 0;
	}	
};

class connectedC{
	public: 
		int numRows;
	    int numCols;
	    int minVal; 
	    int maxVal; 
	    int newMin; 
		int newMax;
		
		int newLable;
		int newLableUsed;
		int **zeroFramedArr;   
		int *EQAry;
		int neighborAry[5];		

		Property *pArray;
		
		connectedC(int row, int col, int min, int max) {
			
			newLable = 0;
			numRows = row;
			numCols = col;
			minVal = min;
			maxVal = max;
			newLableUsed = 0;
			zeroFramedArr = new int*[row+2];
			for(int i = 0; i < row+2; i++) {
				zeroFramedArr[i] = new int[col+2];
			}
			
			EQAry = new int[(row*col)/4];
			for(int i = 0; i < (row*col)/4; i++) {
				EQAry[i] = i;
			}	
		}

		void zeroFramed() {
			for(int i = 0; i < numRows+2; i++){
				zeroFramedArr[i][0] = 0;
				zeroFramedArr[i][numCols+1] = 0;
			}
			for(int j = 0; j < numCols+2; j++){
				zeroFramedArr[0][j] = 0;
				zeroFramedArr[numRows+1][j] = 0;
			}		
		} 
		
		void loadImage(ifstream &fileIn) {
			int pixel_val = 0;
			int i = 1, j = 1;
			while(fileIn >> pixel_val) {	
				
				if(j > numCols) {
					i++;
					j = 1;
				}
				zeroFramedArr[i][j] = pixel_val;
				j++;	
			}	
		}
		
		void loadNeighbors(int row, int col, int pass) {
			if(pass == 1) {
				neighborAry[0] = zeroFramedArr[row-1][col-1];
				neighborAry[1] = zeroFramedArr[row-1][col];
				neighborAry[2] = zeroFramedArr[row-1][col+1];
				neighborAry[3] = zeroFramedArr[row][col-1];
			}
			
			else {
				neighborAry[4] = zeroFramedArr[row][col];
				neighborAry[0] = zeroFramedArr[row+1][col+1];
				neighborAry[1] = zeroFramedArr[row+1][col];
				neighborAry[2] = zeroFramedArr[row+1][col-1];
				neighborAry[3] = zeroFramedArr[row][col+1];
			}
		}
	
		void CCPassOne(ofstream &outFile1) {
			
			for(int i = 1; i < numRows+1; i++) {
				for(int j = 1; j < numCols+1; j++) {
					if(zeroFramedArr[i][j] > 0)
					{
						loadNeighbors(i,j,1);
						int min = 50, max = 0; 

						for(int k = 0; k < 4; k++) {
							if(neighborAry[k] == 0) {
								continue;
							}
							if(neighborAry[k] < min) {
								min = neighborAry[k];
							}
							if(neighborAry[k] > max) {
								max = neighborAry[k];
							}
						}

						if	(	neighborAry[0] == 0 &&
								neighborAry[1] == 0 &&
								neighborAry[2] == 0 && 
								neighborAry[3] == 0) {
							
							newLable++;
							zeroFramedArr[i][j] = newLable;
						}
						else if(min == max)    {
							zeroFramedArr[i][j] = min;
						}
						else {
							zeroFramedArr[i][j] = min;
							updateEQAry(min, max);
						}
							
					}
				}
			}
		}
		
		void CCPassTwo(ofstream &outFile1) {
			for(int i = numRows; i > 0; i--) {
				for(int j = numCols; j > 0 ; j--) {
					if(zeroFramedArr[i][j] > 0)
					{
						loadNeighbors(i,j,2);

						int min = 50, max = 0; 
						
						for(int k = 0; k < 5; k++) {
							if(neighborAry[k] == 0) {
								continue;
							}
							if(neighborAry[k] < min) {
								min = neighborAry[k];
							}
							if(neighborAry[k] > max) {
								max = neighborAry[k];
							}
						}
						if	(	neighborAry[0] == 0 && 
								neighborAry[1] == 0 && 
								neighborAry[2] == 0 && 
								neighborAry[3] == 0) {
							
							continue;
						}
						else if(min == max)   {
							continue;
						}
						else {
							zeroFramedArr[i][j] = min;
							updateEQAry(min, neighborAry[0]);
							updateEQAry(min, neighborAry[1]);
							updateEQAry(min, neighborAry[2]);
							updateEQAry(min, neighborAry[3]);
							updateEQAry(min, neighborAry[4]);
						}			
					}
				}
			}
		}
	
		void CCPassThree(ofstream &outFile1) {
			for(int i = 1; i < numRows+1; i++) {
				for(int j = 1; j < numCols+1; j++) {
					if(zeroFramedArr[i][j] > 0) {
						zeroFramedArr[i][j] = EQAry[zeroFramedArr[i][j]];
					}
				}
			}
		}
		
		void updateEQAry(int min, int max) {
			EQAry[max] = min; 
		}
		
		void manageEQAry() {
			for(int i = 1; i <= newLable; i++) {
				if(i == EQAry[i]) {
					newLableUsed++;
					EQAry[i] = newLableUsed;
				}
				else {
					EQAry[i] = EQAry[EQAry[i]];
				}	
			}
		}
		
		void binaryImage(ofstream &outFile1) {
			outFile1 << numRows << " " << numCols << " " << 0 << " " << newLableUsed << endl; 
			for(int i = 1; i < numRows+1; i++) {
				for(int j = 1; j < numCols+1; j++) {
					outFile1 << zeroFramedArr[i][j] << " ";
				}
				outFile1 << endl;
			}
		}
		
		void findCCProperties() {
		
		pArray = new Property[newLableUsed+1];
		for(int i = 0; i <= newLableUsed; i++) pArray[i] = Property();
		
		int count = 1;
		
		while (count <= newLableUsed) {
			
			pArray[count].minRow = numRows;
			pArray[count].minCol = numCols;
			
			for (int r = 1; r <= numRows; r++) {
				for (int c = 1; c <= numCols; c++) {
					if (zeroFramedArr[r][c] == count) {
						
						if(pArray[count].minRow > r) pArray[count].minRow = r;
						if(pArray[count].minCol > c) pArray[count].minCol = c;
						if(pArray[count].maxRow < r) pArray[count].maxRow = r;
						if(pArray[count].maxCol < c) pArray[count].maxCol = c;	
						
						pArray[count].ccLabel = count;
						pArray[count].noOfPixels++;	
					}	
				}
			}
			
			if (pArray[count].minRow > 0) pArray[count].minRow--;
			if (pArray[count].minCol > 0) pArray[count].minCol--;
			if (pArray[count].maxRow > 0) pArray[count].maxRow--;
			if (pArray[count].maxCol > 0) pArray[count].maxCol--;
			count++;
		}	
	}
	
	void printProperties(ofstream &outFile2) {
		outFile2 << newLableUsed << endl;
		for(int i = 1; i <= newLableUsed; i++) {
			outFile2 << pArray[i].ccLabel << " " << pArray[i].minRow << " " << pArray[i].minCol << " " << pArray[i].maxRow << " " << pArray[i].maxCol << " " << pArray[i].noOfPixels << endl;
		}	
	}	
		
};


int main(int argc, char*argv[]) {
	ifstream fileIn;
	ofstream outFile1;
	ofstream outFile2;
	
	fileIn.open(argv[1]);
	outFile1.open(argv[2]);
	outFile2.open(argv[3]);
	int row, col, max, min;
	fileIn >> row >> col >> min >> max;
	
	connectedC cc(row, col, min, max);
	cc.loadImage(fileIn);
	cc.zeroFramed();
	cc.CCPassOne(outFile1);
	cc.CCPassTwo(outFile1);
	cc.manageEQAry();

	cc.CCPassThree(outFile1);
	cc.binaryImage(outFile1);
	cc.findCCProperties();
	cc.printProperties(outFile2);
	
	fileIn.close();
	outFile1.close();
	outFile2.close();
}
	
