# Example 3-9. Computing square roots with the calculator

### 실행 예제

```
> let avg(a,b)=(a+b)/2;
Defined avg
> let sq(n)=e=1; while |((t=n/e)-e)>.001 do e=avg(e,t);;
Defined sq
> sq(10)
= 3.162
> sqrt(10)
= 3.162
> sq(10)-sqrt(10) // accurate to better than the .001 cutoff */
= 0.000178
```

### 기능 테스트용

```
let sq(n)=e=1; while |((t=n/e)-e)>.001 do e=avg(e,t);;
let avg(a,b)=(a+b)/2;
sq(10)
sqrt(10)
sq(10)-sqrt(10) /* accurate to better than the .001 cutoff */
```

### 주석 테스트용

```
abc=24
```

```
abc /* sdfasdfdsf*
adfasdf*  /asdlfkjasldfj */
```

```
abc /* adfasdf /* adslfkjasldf */
```

```
abc /* adfasdf /* adslfkjasldf */ adsfasdf */
```

### 다중인자 함수

```
let avg(a,b,c)=k=0; while k < 5 do k=k+1; a+b+c+k;;
```

```
> let avg(a,b,c)=k=0; while k < 5 do k=k+1; a+b+c+k;;
Defined avg
> avg(1,2,3)
=   11
```


```
let avg(a,b,c)=i=0;k=0; while i < 3 do i=i+1; k=k+(a+b+c);; k/2;
```

```
> /* avg(1,2,3) => v=(1+2+3), i * v / 2, i < 3 인경우 i = 3 */
> let avg(a,b,c)=i=0;k=0; while i < 3 do i=i+1; k=k+(a+b+c);; k/2;
Defined avg
> avg(1,2,3)
=    9
```

### 테스트용

```
debug_flag on
l_debug_flag on
y_debug_flag on
```
a = 8
b = 3
let fn1(n)=k=1;k=k+log(n);
if ( sqrt(a) > b ) then c = 3; else c = fn1(5); 
```



