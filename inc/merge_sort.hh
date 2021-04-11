

//Merge Sort - O(nlogn)  
//Taken from http://www.thelearningpoint.net/computer-science/sorting-algorithms/merge-sort---c-program

void Merge(struct data *array, struct data *tempArray, int left, int mid, int right)
{
  /*We need a Temporary array to store the new sorted part*/
  //struct data *tempArray = calloc(right-left+1, sizeof(struct data));
  int pos=0,lpos = left,rpos = mid + 1;
  while(lpos <= mid && rpos <= right)
  {
    if(array[lpos].time < array[rpos].time)
    {
      tempArray[pos++] = array[lpos++];
    }
    else
    {
      tempArray[pos++] = array[rpos++];
    }
  }
  while(lpos <= mid)  tempArray[pos++] = array[lpos++];
  while(rpos <= right)tempArray[pos++] = array[rpos++];
  int iter;
  /* Copy back the sorted array to the original array */
  for(iter = 0;iter < pos; iter++)
  {
    array[iter+left] = tempArray[iter];
  }
  return;
}


void MergeSort(struct data *array, struct data *tempArray, int left, int right)
{
  int mid = (left+right)/2;
  /* We have to sort only when left<right because when left=right it is anyhow sorted*/
  if(left<right)
  {
    /* Sort the left part */
    MergeSort(array, tempArray, left,mid);
    /* Sort the right part */
    MergeSort(array, tempArray,mid+1,right);
    /* Merge the two sorted parts */
    Merge(array,tempArray,left,mid,right);
  }
}
/* Merge functions merges the two sorted parts. Sorted parts will be from [left, mid] and [mid+1, right].
 */


 
