!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! sorting.h v1.2, 17 Sep 2003
! Sort functions for Inform. Written by Fredrik Ramsberg. 
! Thanks to Neil Cerutti and Roger Firth for submitting bug reports.
! Freely usable by anyone for any purpose. I request but don't demand
! that you give due credit if this code is used in something that is 
! released to the public. I'd also appreciate if you'd mail me bug 
! reports or your opinions if you use this little library. 
!   -- Fredrik Ramsberg, fredrik {AT} ramsberg {DOT} net
!
! Publically usable routines:
!
! QUICKSORT group
! ---------------
! inssort_bytes_idx(arr, index, from, to);
! inssort_words_idx(arr, index, from, to [, comparison]);
! qsort_bytes_idx(arr, index, from, to);
! qsort_words_idx(arr, index, from, to [, comparison]);
! inssort_bytes(arr, from, to);
! inssort_words(arr, from, to [, comparison]);
! qsort_bytes(arr, from, to);
! qsort_words(arr, from, to [, comparison]);
!
! SHELLSORT group
! ---------------
! shsort_bytes_idx(arr, index, from, to);
! shsort_words_idx(arr, index, from, to [, comparison]);
! shsort_bytes(arr, from, to);
! shsort_words(arr, from, to [, comparison]);
!
! Which one of these three sort algorithms is the fastest, depends
! on the data to be sorted. Here's a short guide:
!
! Use InsertionSort (inssort) when no value is expected to be far from 
! its correct position (up to roughly ten positions away is fine). Many 
! small disorders are quite ok. Also use InsertionSort when the sorting 
! has to be stable. This means that if there are two equal values in 
! the data, the one that is to the left of the other when the sorting 
! starts must still be to the left of the other when the sorting ends. 
! This is not often a demand, but it happens.
!
! Use QuickSort (qsort) when the data is expected to be entirely unsorted.
!
! Use ShellSort (shsort) when the data may or may not be nearly in order,
! when there may or may not be elements that are far from their correct
! position, and when you can't decide which algorithm is the best. It's
! a very good allround algorithm.
!
! The InsertionSort algorithm is very closely related to ShellSort.
! However, the InsertionSort functions are in the QuickSort group in this
! library. This is only because this particular QuickSort implementation 
! utilises InsertionSort in the final stages of sorting, for better speed. 
! This means that the InsertionSort routines can't be removed if one wants 
! to use the QuickSort routines.
!
! All functions that take an index argument are called index-based sort
! functions. These will not alter the order of the original array, but
! will instead build an index on the side that says in which order the
! array elements should be. The index entries are always word-sized,
! regardless of the size of the data elements. This is quite powerful
! because it means you can keep track of related data on the side, i.e.
! if you sort an array containing people's salaries, you will still know
! what person had what salary even after sorting.
!
! All sort-functions that are not index-based are referred to as direct sort
! functions.
!
! The 'from' argument should contain the index of the first element to be
! sorted, usually 0 if the data is in an ordinary array, or 1 if it's in
! a table. The 'to' argument should hold the index of the last element to 
! be sorted.
!
! For functions that take an optional comparison argument, this argument is
! expected to be a function that takes two arguments, a and b, and which
! returns a non-zero value iff a should end up after b in the sorted array.
! Important: Such a function must never return a non-zero value if a and b
! are equivalent, from a sorting point of view.
! Sample function to sort an array of words in descending order:
! [ lessthan a b; return a<b; ];
! Sample sorting call that utilises the sample function above:
! qsort_words(my_word_table, 1, my_word_table-->0, lessthan);
!
! All functions that sort word-data use the standard Inform signed comparison
! by default. If unsigned comparison is required, supply your own comparison
! function instead.
!
! To stop some of the routines from compiling (in order to save space), one 
! can set one or more of the following constants, before including this file:
! NOQUICKSORT, NOSHELLSORT,
! NOINDEXSORT, NODIRECTSORT,
! NOBYTESORT, NOWORDSORT
! If no routines are removed in this manner, this package will take up roughly
! 3.5 KB in the story file, with debugging information turned off.
!
! There are two more constants which may be defined before including this file:
! QUICKSORTLIMIT
! SHELLSORTCONST
! If you're _really_ interested in fine-tuning your sorting, these constants
! may be of interest to you. Otherwise, feel free to forget about them right
! away. If any of them are defined, it must be set to a value greater than
! zero, or the sorting routines will crash. Any value higher than 20 is
! highly unlikely to be useful. Which values are the best (i.e. giving the
! fastest sorting) depends on which interpreter will be used to run the
! program, as well as the  nature of the data to be sorted.
! The programmer is free to define both constants, define one of them, or
! leave both undefined. Leaving them undefined will set them to values
! optimised for sorting random data under WinFrotz (QUICKSORTLIMIT=10,
! SHELLSORTCONST=3), and this is generally good enough for any data under any
! interpreter. Of course, QUICKSORTLIMIT only affects QuickSort, and
! SHELLSORTCONST only affects ShellSort.
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

System_file;

#Ifndef NOQUICKSORT;

Default QUICKSORTLIMIT 10;

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! Index-based InsertionSort and QuickSort routines.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#Ifndef NOINDEXSORT;

#Ifndef NOBYTESORT;

[ _isbi_sub arr index from to   i j v vi f1;
  f1=from + 1;
  for(i=f1, j=i, vi=index-->i, v=arr->vi : i<=to : index-->(j+1)=vi, i++, j=i, vi=index-->i, v=arr->vi )
    while(j-- >= f1  && arr->(index-->j) > v)
      index-->(j+1)=index-->j;
];

[ inssort_bytes_idx arr index from to   i;
  for(i=from:i<=to:i++)
    index-->i=i;
  _isbi_sub(arr, index, from, to);
];

[ _qsbi_sub arr index from to   i j m temp vm vt p;
  while(from>=0) {
    i=from-1;
    j=to;
    m=from+(to-from)/2;
    p=arr->(index-->from);
    vm=arr->(index-->m);
    vt=arr->(index-->to);
    if((p<=vm && vm<=vt) || (vt<=vm && vm<=p)) {
        p=vm;
        temp=index-->m;
        index-->m=index-->from;
        index-->from=temp;
    } else if((p<=vt && vt<=vm) || (vm<=vt && vt<=p)) {
        p=vt;
        temp=index-->to;
        index-->to=index-->from;
        index-->from=temp;
    }
    
    while(++i<=j) {
      if(arr->(index-->i)>p) {
        while(arr->(index-->j)>p && --j>i);
        if(j>i) {
          temp=index-->j; index-->j=index-->i; index-->i=temp;
        } else
          i--;
      }
    }
    i--;
    temp=index-->i;
    index-->i=index-->from;
    index-->from=temp;

    if(i-from>to-i) {
      if(to-i>QUICKSORTLIMIT)
        _qsbi_sub(arr, index, i+1, to);
      if(i-from>QUICKSORTLIMIT)
        to=i-1;
      else
        from=-1;
    }
    else {    
      if(i-from>QUICKSORTLIMIT)
        _qsbi_sub(arr, index, from, i-1);
      if(to-i>QUICKSORTLIMIT)
        from=i+1;
      else
        from=-1;
    }  
  }
];

[ qsort_bytes_idx arr index from to   i;
  for(i=from:i<=to:i++)
    index-->i=i;
  _qsbi_sub(arr, index, from, to);
  #Iftrue QUICKSORTLIMIT > 1;
  _isbi_sub(arr, index, from, to);
  #Endif;
];

#Endif; ! Not NOBYTESORT;

#Ifndef NOWORDSORT;

[ _iswi_sub arr index from to   i j v vi f1;
  f1=from + 1;
  for(i=f1, j=i, vi=index-->i, v=arr-->vi : i<=to : index-->(j+1)=vi, i++, j=i, vi=index-->i, v=arr-->vi )
    while(j-- >= f1  && arr-->(index-->j) > v)
      index-->(j+1)=index-->j;
];

[ _isgi_sub arr index from to comp   i j v vi f1;
  f1=from + 1;
  for(i=f1, j=i, vi=index-->i, v=arr-->vi : i<=to : index-->(j+1)=vi, i++, j=i, vi=index-->i, v=arr-->vi )
    while(j-- >= f1  && comp(arr-->(index-->j), v))
      index-->(j+1)=index-->j;
];

[ inssort_words_idx arr index from to comp  i;
  for(i=from:i<=to:i++)
    index-->i=i;
  if(comp)
    return _isgi_sub(arr, index, from, to, comp);
  _iswi_sub(arr, index, from, to);
];

[ _qswi_sub arr index from to   i j m temp vm vt p;
  while(from>=0) {
    i=from-1;
    j=to;
    m=from+(to-from)/2;
    p=arr-->(index-->from);
    vm=arr-->(index-->m);
    vt=arr-->(index-->to);
    if((p<=vm && vm<=vt) || (vt<=vm && vm<=p)) {
        p=vm;
        temp=index-->m;
        index-->m=index-->from;
        index-->from=temp;
    } else if((p<=vt && vt<=vm) || (vm<=vt && vt<=p)) {
        p=vt;
        temp=index-->to;
        index-->to=index-->from;
        index-->from=temp;
    }
    
    while(++i<=j) {
      if(arr-->(index-->i)>p) {
        while(arr-->(index-->j)>p && --j>i);
        if(j>i) {
          temp=index-->j; index-->j=index-->i; index-->i=temp;
        } else
          i--;
      }
    }
    i--;
    temp=index-->i;
    index-->i=index-->from;
    index-->from=temp;

    if(i-from>to-i) {
      if(to-i>QUICKSORTLIMIT)
        _qswi_sub(arr, index, i+1, to);
      if(i-from>QUICKSORTLIMIT)
        to=i-1;
      else
        from=-1;
    }
    else {    
      if(i-from>QUICKSORTLIMIT)
        _qswi_sub(arr, index, from, i-1);
      if(to-i>QUICKSORTLIMIT)
        from=i+1;
      else
        from=-1;
    }  
  }
];

[ _qsgi_sub arr index from to comp   i j m temp vm vt p;
  while(from>=0) {
    i=from-1;
    j=to;
    m=from+(to-from)/2;
    p=arr-->(index-->from);
    vm=arr-->(index-->m);
    vt=arr-->(index-->to);
    if(~~(((comp(p,vm)) || comp(vm,vt)) && ((comp(vt,vm)) || comp(vm,p)))) {
        p=vm;
        temp=index-->m;
        index-->m=index-->from;
        index-->from=temp;
    } else if(~~(((comp(p,vt)) || comp(vt,vm)) && ((comp(vm,vt)) || comp(vt,p)))) {
        p=vt;
        temp=index-->to;
        index-->to=index-->from;
        index-->from=temp;
    }
    
    while(++i<=j) {
      if(comp(arr-->(index-->i),p)) {
        while(comp(arr-->(index-->j),p) && --j>i);
        if(j>i) {
          temp=index-->j; index-->j=index-->i; index-->i=temp;
        } else
          i--;
      }
    }
    i--;
    temp=index-->i;
    index-->i=index-->from;
    index-->from=temp;

    if(i-from>to-i) {
      if(to-i>QUICKSORTLIMIT)
        _qsgi_sub(arr, index, i+1, to, comp);
      if(i-from>QUICKSORTLIMIT)
        to=i-1;
      else
        from=-1;
    }
    else {    
      if(i-from>QUICKSORTLIMIT)
        _qsgi_sub(arr, index, from, i-1, comp);
      if(to-i>QUICKSORTLIMIT)
        from=i+1;
      else
        from=-1;
    }  
  }
];

[ qsort_words_idx arr index from to comp   i;
  for(i=from:i<=to:i++)
    index-->i=i;
  if(comp) {
    _qsgi_sub(arr, index, from, to, comp);
    #Iftrue QUICKSORTLIMIT > 1;
    _isgi_sub(arr, index, from, to, comp);
    #Endif;
  } else {
    _qswi_sub(arr, index, from, to);
    #Iftrue QUICKSORTLIMIT > 1;
    _iswi_sub(arr, index, from, to);
    #Endif;
  }
];

#Endif; ! Not NOWORDSORT;

#Endif; ! Not NOINDEXSORT

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! Direct InsertionSort and QuickSort routines.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#Ifndef NODIRECTSORT;

#Ifndef NOBYTESORT;

[ inssort_bytes arr from to   i j v f1;
  f1=from + 1;
  for(i=f1, j=i, v=arr->i : i<=to : arr->(j+1)=v, i++, j=i, v=arr->i)
    while(j-- >= f1  && arr->j > v)
      arr->(j+1)=arr->j;
];

[ _qsb_sub arr from to   i j m temp vm vt p;
  while(from>=0) {
    i=from-1;
    j=to;
    m=from+(to-from)/2;
    p=arr->from;
    vm=arr->m;
    vt=arr->to;
    if((p<=vm && vm<=vt) || (vt<=vm && vm<=p)) {
        p=vm;
        temp=arr->m;
        arr->m=arr->from;
        arr->from=temp;
    } else if((p<=vt && vt<=vm) || (vm<=vt && vt<=p)) {
        p=vt;
        temp=arr->to;
        arr->to=arr->from;
        arr->from=temp;
    }
    
    while(++i<=j) {
      if(arr->i > p) {
        while(arr->j > p && --j>i);
        if(j>i) {
          temp=arr->j; arr->j=arr->i; arr->i=temp;
        } else
          i--;
      }
    }
    i--;
    temp=arr->i;
    arr->i=arr->from;
    arr->from=temp;

    if(i-from>to-i) {
      if(to-i>QUICKSORTLIMIT)
        _qsb_sub(arr, i+1, to);
      if(i-from>QUICKSORTLIMIT)
        to=i-1;
      else
        from=-1;
    }
    else {    
      if(i-from>QUICKSORTLIMIT)
        _qsb_sub(arr, from, i-1);
      if(to-i>QUICKSORTLIMIT)
        from=i+1;
      else
        from=-1;
    }  
  }
];

[ qsort_bytes arr from to;
  _qsb_sub(arr, from, to);
  #Iftrue QUICKSORTLIMIT > 1;
  inssort_bytes(arr, from, to);
  #Endif;
];

#Endif; ! Not NOBYTESORT;

#Ifndef NOWORDSORT;

[ _isg_sub arr from to comp   i j v f1;
  f1=from + 1;
  for(i=f1, j=i, v=arr-->i : i<=to : arr-->(j+1)=v, i++, j=i, v=arr-->i)
    while(j-- >= f1  && comp(arr-->j, v))
      arr-->(j+1)=arr-->j;
];

[ inssort_words arr from to comp   i j v f1;
  if(comp)
    return _isg_sub(arr, from, to, comp);
  f1=from + 1;
  for(i=f1, j=i, v=arr-->i : i<=to : arr-->(j+1)=v, i++, j=i, v=arr-->i)
    while(j-- >= f1  && arr-->j > v)
      arr-->(j+1)=arr-->j;
];

[ _qsw_sub arr from to   i j m temp vm vt p;
  while(from>=0) {
    i=from-1;
    j=to;
    m=from+(to-from)/2;
    p=arr-->from;
    vm=arr-->m;
    vt=arr-->to;
    if((p<=vm && vm<=vt) || (vt<=vm && vm<=p)) {
        p=vm;
        temp=arr-->m;
        arr-->m=arr-->from;
        arr-->from=temp;
    } else if((p<=vt && vt<=vm) || (vm<=vt && vt<=p)) {
        p=vt;
        temp=arr-->to;
        arr-->to=arr-->from;
        arr-->from=temp;
    }
    
    while(++i<=j) {
      if(arr-->i > p) {
        while(arr-->j > p && --j>i);
        if(j>i) {
          temp=arr-->j; arr-->j=arr-->i; arr-->i=temp;
        } else
          i--;
      }
    }
    i--;
    temp=arr-->i;
    arr-->i=arr-->from;
    arr-->from=temp;

    if(i-from>to-i) {
      if(to-i>QUICKSORTLIMIT)
        _qsw_sub(arr, i+1, to);
      if(i-from>QUICKSORTLIMIT)
        to=i-1;
      else
        from=-1;
    }
    else {    
      if(i-from>QUICKSORTLIMIT)
        _qsw_sub(arr, from, i-1);
      if(to-i>QUICKSORTLIMIT)
        from=i+1;
      else
        from=-1;
    }  
  }
];

[ _qsg_sub arr from to comp   i j m temp vm vt p;
  while(from>=0) {
    i=from-1;
    j=to;
    m=from+(to-from)/2;
    p=arr-->from;
    vm=arr-->m;
    vt=arr-->to;
    if(~~(((comp(p,vm)) || comp(vm,vt)) && ((comp(vt,vm)) || comp(vm,p)))) {
        p=vm;
        temp=arr-->m;
        arr-->m=arr-->from;
        arr-->from=temp;
    } else if(~~(((comp(p,vt)) || comp(vt,vm)) && ((comp(vm,vt)) || comp(vt,p)))) {
        p=vt;
        temp=arr-->to;
        arr-->to=arr-->from;
        arr-->from=temp;
    }
    
    while(++i<=j) {
      if(comp(arr-->i,p)) {
        while(comp(arr-->j,p) && --j>i);
        if(j>i) {
          temp=arr-->j; arr-->j=arr-->i; arr-->i=temp;
        } else
          i--;
      }
    }
    i--;
    temp=arr-->i;
    arr-->i=arr-->from;
    arr-->from=temp;

    if(i-from>to-i) {
      if(to-i>QUICKSORTLIMIT)
        _qsg_sub(arr, i+1, to, comp);
      if(i-from>QUICKSORTLIMIT)
        to=i-1;
      else
        from=-1;
    }
    else {    
      if(i-from>QUICKSORTLIMIT)
        _qsg_sub(arr, from, i-1, comp);
      if(to-i>QUICKSORTLIMIT)
        from=i+1;
      else
        from=-1;
    }  
  }
];

[ qsort_words arr from to comp;
  if(comp) {
    _qsg_sub(arr, from, to, comp);
    #Iftrue QUICKSORTLIMIT > 1;
    _isg_sub(arr, from, to, comp);
    #Endif;
  } else {
    _qsw_sub(arr, from, to);
    #Iftrue QUICKSORTLIMIT > 1;
    inssort_words(arr, from, to);
    #Endif;
  }
];

#Endif; ! Not NOWORDSORT;

#Endif; ! Not NODIRECTSORT

#Endif; ! Not NOQUICKSORT




#Ifndef NOSHELLSORT;

Default SHELLSORTCONST 3;

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! Index-based ShellSort routines.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#Ifndef NOINDEXSORT;

#Ifndef NOBYTESORT;

[ shsort_bytes_idx arr index from to   i vi j v h k len;
  h=1;
  len=to-from+1;
  while(h < len)
    h=SHELLSORTCONST*h+1;
  for(i=from:i<=to:i++)
    index-->i=i;
  while((h=(h-1)/SHELLSORTCONST)>=1)
    for(k=0: k<h: k++)
      for(i=k+h+from: i<=to: i=i+h) {
        j=i;
        vi=index-->i;
        v=arr->vi;
        while((j=j-h) >= from && arr->(index-->j) > v)
            index-->(j+h)=index-->j;
        index-->(j+h)=vi;
      }
];

#Endif; ! Not NOBYTESORT;

#Ifndef NOWORDSORT;

[ _ssgi_sub arr index from to comp h   i vi j v k;
  while((h=(h-1)/SHELLSORTCONST)>=1)
    for(k=0: k<h: k++)
      for(i=k+h+from: i<=to: i=i+h) {
        j=i;
        vi=index-->i;
        v=arr-->vi;
        while((j=j-h) >= from && comp(arr-->(index-->j), v))
            index-->(j+h)=index-->j;
        index-->(j+h)=vi;
      }
];

[ shsort_words_idx arr index from to comp   h i vi j v k len;
  for(i=from:i<=to:i++)
    index-->i=i;
  h=1;
  len=to-from+1;
  while(h < len)
    h=SHELLSORTCONST*h+1;
  if(comp)
    return _ssgi_sub(arr, index, from, to, comp, h);
  while((h=(h-1)/SHELLSORTCONST)>=1)
    for(k=0: k<h: k++)
      for(i=k+h+from: i<=to: i=i+h) {
        j=i;
        vi=index-->i;
        v=arr-->vi;
        while((j=j-h) >= from && arr-->(index-->j) > v)
            index-->(j+h)=index-->j;
        index-->(j+h)=vi;
      }
];

#Endif; ! Not NOWORDSORT;

#Endif; ! Not NOINDEXSORT;


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! Direct ShellSort routines.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#Ifndef NODIRECTSORT;

#Ifndef NOBYTESORT;

[ shsort_bytes arr from to   i j v h k len;
  h=1;
  len=to-from+1;
  while(h < len)
    h=SHELLSORTCONST*h+1;
  while((h=(h-1)/SHELLSORTCONST)>=1)
    for(k=0: k<h: k++)
      for(i=k+h+from: i<=to: i=i+h) {
        j=i;
        v=arr->i;
        while((j=j-h) >= from && arr->j > v)
            arr->(j+h)=arr->j;
        arr->(j+h)=v;
      }
];

#Endif; ! Not NOBYTESORT;

#Ifndef NOWORDSORT;

[ _ssg_sub arr from to comp h   i j v k;
  while((h=(h-1)/SHELLSORTCONST)>=1)
    for(k=0: k<h: k++)
      for(i=k+h+from: i<=to: i=i+h) {
        j=i;
        v=arr-->i;
        while(((j=j-h) >= from) && comp(arr-->j, v))
            arr-->(j+h)=arr-->j;
        arr-->(j+h)=v;
      }
];

[ shsort_words arr from to comp   i j v h k len;
  h=1;
  len=to-from+1;
  while(h < len)
    h=SHELLSORTCONST*h+1;
  if(comp)
    return _ssg_sub(arr, from, to, comp, h);
  while((h=(h-1)/SHELLSORTCONST)>=1)
    for(k=0: k<h: k++)
      for(i=k+h+from: i<=to: i=i+h) {
        j=i;
        v=arr-->i;
        while((j=j-h) >= from && arr-->j > v)
            arr-->(j+h)=arr-->j;
        arr-->(j+h)=v;
      }
];

#Endif; ! Not NOWORDSORT;

#Endif; ! Not NODIRECTSORT;

#Endif; ! Not NOSHELLSORT

