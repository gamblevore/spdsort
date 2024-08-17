// spdsort.cpp  http://gamblevore.org, by theodore h. smith
// basically... just testing some sorting algorithms...

// my spdsort is like a normal quicksort, but it has better characteristics:
	// 1) handles invalid sort functions. You need this if you are making a programming language, cos you don't want some kid making his first game to crash his computer just cos the sort function is a little bit off. Garbage in = garbage out... thats all we want. C++ sorts will actually crash your app with invalid sort functions.
	// 2) Seems a lil faster?
	// 3) Handles sorted and reverse sorted arrays very fast
	// 4) Code is available and not too big. (unlike C++ sort)
	// 5) Only calls the compare function once... this is important for very slow compare functions. C++ sort calls it twice per-pair of items.
	// 6) It doesn't use a garbage design for the return value. It uses an int, which can store equal, less or more. C++'s sort uses a BOOL. Which... its just not right. If you are depth-sorting some objects, it can lead to flicker as equally distant objects will flicker onscreen.
	// 7) Actually its done so you can return a bool or an int... up to you. We just compare if the result is <= 0.
	// this is true for 0 and -1.

// you'll have to modify this yourself to be useful, of course.
// How to test this on suffix arrays... not sure how?




#include "spdsort.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <chrono>
#include <algorithm>
#include <vector>
#include <random>

#define debugger __builtin_trap();
using std::vector;

void QuickSortNR (int* arr, int elements) {
	#define  MAX_LEVELS  64 // does not recurse...
	int  beg[MAX_LEVELS], end[MAX_LEVELS];
	
	beg[0]=0; end[0]=elements;
	for (int i = 0; i>=0;) {
		int L=beg[i];
		int R=end[i]-1;
		if (L < R) {
			int piv = arr[L];
			while (L<R) {
				while (arr[R]>=piv && L<R) R--; if (L<R) arr[L++]=arr[R];
				while (arr[L]<=piv && L<R) L++; if (L<R) arr[R--]=arr[L];
			}
			arr[L]=piv; beg[i+1]=L+1; end[i+1]=end[i]; end[i++]=L;
			if (end[i]-beg[i] > end[i-1]-beg[i-1]) {
				int swap;
				swap=beg[i]; beg[i]=beg[i-1]; beg[i-1]=swap;
				swap=end[i]; end[i]=end[i-1]; end[i-1]=swap;
			}
		} else {
			i--;
		}
	}
}


bool CSortComparer    (int A, int B)  { return A < B; }
bool CSortComparerRev (int A, int B)  { return A > B; }
void QuickSortCpp (int* self, int N, bool backwards=false) {
	if (backwards)
		std::sort(self, self + N, CSortComparerRev);
	  else
		std::sort(self, self + N, CSortComparer);
}


vector<int> intsReference;
vector<int> ints;
const char* CurrOp = "";
int RunReps = 3;


struct Time {
	std::chrono::system_clock::time_point Start;
	const char* Name;
	Time (const char* name) {
		Start = std::chrono::system_clock::now();
		Name = name;
	}
	~Time () {
		auto EndTime = std::chrono::system_clock::now();
		auto Durr    = std::chrono::duration<double>(EndTime-Start);
		auto Seconds = (Durr*1000.0).count();
		printf("%s %s took %.3fms\n", CurrOp, Name, Seconds);
	}
};



void Validate (const char* name) {
	for (int i = 0; i < ints.size(); i++) {
		if (intsReference[i]!=ints[i]) {
			fprintf(stderr, "%s %s failed", CurrOp, name);
			debugger;
			return;
		}
	}
}


void RunOnce (vector<int>& Src,  const char* name) {
	puts("");
	int n = (int)Src.size();
	CurrOp = name;
	for (int i = 0; i < RunReps; i++) {
		ints = Src;
		auto T = Time("Cpp");
		QuickSortCpp(&ints[0], n);
		Validate(T.Name);
	}
	for (int i = 0; i < RunReps; i++) {
		ints = Src;
		auto T = Time("SPD");
		SpdSort(0, &ints[0], &ints[n-1]);
		Validate(T.Name);
	}
	for (int i = 0; i < RunReps; i++) {
		ints = Src;
		auto T = Time("non-recursive"); // from https://alienryderflex.com/quicksort/
		QuickSortNR(&ints[0], n);
		Validate(T.Name);
	}
}



int main (int argc, const char** argv) {
	if (!argv[1]) {
		printf("usage: spd_sort nSize nReps\n");
		exit(0);
	}
		
	int n = atoi(argv[1]);
	if (n <= 0 or n >= 100000000) {
		fprintf(stderr, "invalid nsize %s\n", argv[1]);
		exit(0);
	}
	if (argv[2])
		RunReps = atoi(argv[2]);
	if (RunReps <= 0 or RunReps > 1000) {
		fprintf(stderr, "invalid nReps %s\n", argv[2]);
		exit(0);
	}
	printf("Sorting %i ints\n", n);
	vector<int> Src(n);
    std::mt19937 rng(123);
    std::uniform_int_distribution<std::mt19937::result_type> rnd(0, n*2);
	for (int i = 0; i < n; i++) {
		Src[i] = rnd(rng);
	}

	///
	intsReference = Src;
	QuickSortCpp(&intsReference[0], n);
	RunOnce(Src, "random");

	///
	QuickSortCpp(&Src[0], n);
	RunOnce(Src, "sorted");
	
	
	///
	QuickSortCpp(&Src[0], n, true);
	RunOnce(Src, "reverse");

	
	puts("Testing...");

	n = 4000;
	ints.resize(n);
	for (int TestCount = 1000; TestCount >= 0; TestCount--) {
		for (int i = 0; i < n; i++) {
			ints[i] = rnd(rng);
		}
		intsReference = ints;
		QuickSortCpp(&intsReference[0], n);
		SpdSort(0, &ints[0], &ints[n-1]);
		Validate("SPD2");
	}

	puts("Tested ok");
	return 0;
}
