extern int img_width;
extern int img_height;
extern double *int_arr;
extern int ipts_nums;
extern double *ipts_info;
extern int R_w[];
extern int R_h[];

#define GS(x,y,s) ((1.0f/(2.0f*pi*s*s)) * exp( -((x)*(x)+(y)*(y))/(2.0f*s*s)))

#define BOXVALUE(r,c,rs,cs,boxvalue) do{\
int step = img_width; \
int r1=(int)(r-1),c1=(int)(c-1),r2=(int)(r+rs-1),c2=(int)(c+cs-1); \
double A=0.0,B=0.0,C=0.0,D=0.0; \
if(img_height-1 < r1) \
	r1 = img_height -1 ; \
if(img_width-1 < c1) \
	c1 = img_width - 1; \
if(img_height-1 < r2) \
	r2 = img_height -1;\
if(img_width-1 <  c2) \
	c2 = img_width -1; \
if (r1 >= 0 && c1 >= 0) A = int_arr[r1 * step + c1]; \
if (r1 >= 0 && c2 >= 0) B = int_arr[r1 * step + c2]; \
if (r2 >= 0 && c1 >= 0) C = int_arr[r2 * step + c1]; \
if (r2 >= 0 && c2 >= 0) D = int_arr[r2 * step + c2]; \
boxvalue = A-B-C+D; \
if(boxvalue < 0) \
	boxvalue = 0;\
}while(0)

#define HAARX(r,c,s,rs) do{ \
double boxvalue[2]; \
BOXVALUE(r-(s)/2,c,s,(s)/2,boxvalue[0]); \
BOXVALUE(r-(s)/2,c-(s)/2,s,(s)/2,boxvalue[1]); \
rs = boxvalue[0] - boxvalue[1]; \
}while(0)

#define HAARY(r,c,s,rs) do{ \
double boxvalue[2]; \
BOXVALUE(r,c-(s)/2,(s)/2,s,boxvalue[0]); \
BOXVALUE(r-(s)/2,c-(s)/2,(s)/2,s,boxvalue[1]); \
rs = boxvalue[0] - boxvalue[1]; \
}while(0)

#define GETANGLE(x,y,res) do{ \
  double pi =3.14159; \
  if(x > 0 && y >= 0) \
    res = atan(y/x); \
  else if(x < 0 && y >= 0) \
    res = pi - atan(-y/x); \
  else if(x < 0 && y < 0) \
    res = pi + atan(y/x); \
  else if(x > 0 && y < 0) \
    res = 2*pi - atan(-y/x); \
  else \
	res = 0; \
}while(0)

#define GETRES(src,index,type,res) do{\
int r = (index / R_w[src]) * R_step[src]; \
int c = (index % R_w[src]) * R_step[src]; \
int rs_b = (R_filter[src] - 1) / 2; \
int l = (R_filter[src]) / 3;  \
int w = R_filter[src];\
double box[8]; \
double Dxx,Dyy,Dxy; \
double inverse_area = (double)1.0/(w*w);\
BOXVALUE(r - l + 1, c - rs_b,     2*l - 1, w,       box[0]);\
BOXVALUE(r - l + 1, c - l / 2, 2*l - 1, l,       box[1]);\
BOXVALUE(r - rs_b,     c - l + 1, w,       2*l - 1, box[2]);\
BOXVALUE(r - l / 2, c - l + 1, l,       2*l - 1, box[3]);\
BOXVALUE(r - l,c + 1,l,l,box[4]);\
BOXVALUE(r + 1,c - l,l,l,box[5]);\
BOXVALUE(r - l,c - l,l,l,box[6]);\
BOXVALUE(r + 1,c + 1,l,l,box[7]);\
Dxx = box[0] - box[1] *3;\
Dyy = box[2] - box[3] *3;\
Dxy = box[4] + box[5] - box[6] - box[7];\
Dxx *= inverse_area;\
Dyy *= inverse_area;\
Dxy *= inverse_area;\
if(type==0) \
	res = Dxx * Dyy - 0.81 * Dxy * Dxy;\
else if(Dxx + Dyy >=0) \
	res = 1.0; \
else \
	res =0.0; \
} while(0)


// Get image attributes	
const int R_filter[12] = {9,15,21,27,39,51,75,99,147,195,291,387};
const  double thresh = 0.0004;
const int R_step[12] = {2,2,2,2,4,4,8,8,16,16,32,32};

composite getResXY(output stream<double x,double y,double angle>Out,input stream<double x,double y,double scale,
		double laplacian>In)
{
	 const double gauss25 [7][7] = {
	  0.02546481,	0.02350698,	0.01849125,	0.01239505,	0.00708017,	0.00344629,	0.00142946,
	  0.02350698,	0.02169968,	0.01706957,	0.01144208,	0.00653582,	0.00318132,	0.00131956,
	  0.01849125,	0.01706957,	0.01342740,	0.00900066,	0.00514126,	0.00250252,	0.00103800,
	  0.01239505,	0.01144208,	0.00900066,	0.00603332,	0.00344629,	0.00167749,	0.00069579,
	  0.00708017,	0.00653582,	0.00514126,	0.00344629,	0.00196855,	0.00095820,	0.00039744,
	  0.00344629,	0.00318132,	0.00250252,	0.00167749,	0.00095820,	0.00046640,	0.00019346,
	  0.00142946,	0.00131956,	0.00103800,	0.00069579,	0.00039744,	0.00019346,	0.00008024
	};
	 
	 Out = getResXY(In){
		work
		{
			int i,j;
			double gauss;
			double rX=0.0,rY=0.0,angle=0.0,resX,resY;
			int r,c,s,idx=0;
			int id[13] = {6,5,4,3,2,1,0,1,2,3,4,5,6};
			if(In[0].x != -1){
				r = (int)floor(In[0].y + 0.5);
				c = (int)floor(In[0].x + 0.5);
				s = (int)floor(In[0].scale + 0.5);
				for(i=-6;i<6;i++){
					for(j=-6;j<6;j++){
						if(i*i + j*j < 36){
							gauss = gauss25[id[i+6]][id[j+6]];  
							HAARX(r+j*s,c+i*s,4*s,rX);
							HAARY(r+j*s,c+i*s,4*s,rY);
							resX = rX * gauss;
							resY = rY * gauss;
							GETANGLE(resX,resY,angle);
							Out[idx].x = resX;
							Out[idx].y = resY;
							Out[idx].angle = angle;
							idx++;
						}
					}
				}
			}else{
				for(i=0;i<109;i++){
					Out[0].x = 0.0;
					Out[0].y = 0.0;
					Out[0].angle = 0.0;
				}
			}
		}
		window
		{
			In sliding(1,1);
			Out tumbling(109);
		}
	 };
}

composite getSum(output stream<double x>Out, input stream<double x,double y,double angle>In)
{
	const double pi = 3.14159;
	
	Out = getSum(In){
		work
		{
			int k;
			double ang,ang1,ang2;
			double sumX=0.0,sumY=0.0;
			double max =0.0;
			double orien=0.0;
			if(In[0].x != 0.0 && In[0].y != 0.0){
				for(ang1 = 0; ang1 < 2*pi;  ang1+=0.15f){
					ang2 = ( ang1+pi/3.0f > 2*pi ? ang1-5.0f*pi/3.0f : ang1+pi/3.0f);
					sumX=0.0;
					sumY=0.0;
					for(k=0;k<109;k++){
						ang = In[k].angle;
						if (ang1 < ang2 && ang1 < ang && ang < ang2) 
						{
							sumX+=In[k].x;  
							sumY+=In[k].y;
						} 
						else if (ang2 < ang1 &&((ang > 0 && ang < ang2) || (ang > ang1 && ang < 2*pi) )) 
						{
							sumX+=In[k].x;  
							sumY+=In[k].y;
						}
					}
					if (sumX*sumX + sumY*sumY > max) 
					{
						max = sumX*sumX + sumY*sumY;
						GETANGLE(sumX, sumY,orien);
					}
				}
				Out[0].x = orien;
			} else {
				Out[0].x = -1.0;
			}
		}
		window
		{
			In sliding(109,109);
			Out tumbling(1);
		}
	};
}

composite getOrientation(output stream<double x,double y,double scale,double laplacian,double orientation>Out,
						 input stream<double x,double y,double scale,double laplacian>In)
{
	
	stream<double x,double y,double scale,double laplacian> data1,data2;
	stream<double x> orientation;
	
	(data1,data2) = separate(In){
		work
		{
			data1[0].x = In[0].x;
			data1[0].y = In[0].y;
			data1[0].scale = In[0].scale;
			data1[0].laplacian = In[0].laplacian;
			data2[0].x = In[0].x;
			data2[0].y = In[0].y;
			data2[0].scale = In[0].scale;
			data2[0].laplacian = In[0].laplacian;
		}
		window
		{
			In sliding(1,1);
		}
	};
	orientation = pipeline(data1){
		add getResXY();
		add getSum();
	};
	Out = getOrientation(data2,orientation){
		work
		{
			Out[0].x = data2[0].x;
			Out[0].y = data2[0].y;
			Out[0].scale = data2[0].scale;
			Out[0].laplacian = data2[0].laplacian;
			Out[0].orientation = orientation[0].x;
		}
	};
}
//stream成员不能是数组
//69个
composite getDesc(output stream<double x>Out,input stream<double x,double y,double scale,double laplacian,double orientation>In)
{
	param
		int i,int j,double cx,double cy;
	
	int ix =i+5,jx = j+5;
	const double pi = 3.14159;
	Out = getDesc(In){
		work
		{
			double scale, haar_s,dx, dy, mdx, mdy, co, si,gauss_s1,gauss_s2,g_s,rrx,rry;
			double rx,ry;
			int k,l,xs,ys,sample_x,sample_y,g_x,g_y,x,y;
			
			if(In[0].x != -1){
				x = (int)floor(In[0].x + 0.5);
				y = (int)floor(In[0].y + 0.5);
				scale = In[0].scale;
				haar_s = (int)floor(scale+0.5);
				co = cos(In[0].orientation);
				si = sin(In[0].orientation);
				xs = (int)floor(x -jx*scale*si + ix*scale*co + 0.5);
				ys = (int)floor(y + jx*scale*co + ix*scale*si + 0.5);
				dx = dy = mdx = mdy =0;
				for(k=i;k<i+9;++k){
					for(l=j;l<j+9;++l){
						//Get coords of sample point on the rotated axis
						sample_x = (int)floor(x -l*scale*si + k*scale*co + 0.5);
						sample_y = (int)floor(y + l*scale*co + k*scale*si+ 0.5);
						HAARX(sample_y,sample_x,2.5*haar_s,rx);
						HAARY(sample_y,sample_x,2.5*haar_s,ry);
						g_x = xs-sample_x;
						g_y = ys-sample_y;
						g_s = 2.5f*scale;
						gauss_s1 =  GS(g_x,g_y,g_s);
						
						//Get the gaussian weighted x and y responses on rotated axis
						rrx = gauss_s1*(-rx*si + ry*co);
						rry = gauss_s1*(rx*co + ry*si);

						dx += rrx;
						dy += rry;
						mdx += fabs(rrx);
						mdy += fabs(rry);
					}
				}
				//Add the values to the descriptor vector
				gauss_s2 = GS(cx-2.0,cy-2.0,1.5);
				
				Out[0].x = dx*gauss_s2;
				Out[1].x = dy*gauss_s2;
				Out[2].x = mdx*gauss_s2;
				Out[3].x = mdy*gauss_s2;
				Out[4].x = (dx*dx + dy*dy + mdx*mdx + mdy*mdy) * gauss_s2*gauss_s2;
			}else {
				Out[0].x = -1.0;
				Out[1].x = -1.0;
				Out[2].x = -1.0;
				Out[3].x = -1.0;
				Out[4].x = -1.0;
			}
		}
		window
		{
			In sliding(1,1);
			Out tumbling(5);
		}
	};
}
composite getDescriptor(output stream<double x>Out,input stream<double x,double y,double scale,double laplacian,double orientation>In)
{
	stream<double x> Desc;
	stream<double x,double y,double scale,double laplacian,double orientation>data1,data2;
	(data1,data2) = separate(In){
		work
		{
			data1[0].x = In[0].x;
			data1[0].y = In[0].y;
			data1[0].scale = In[0].scale;
			data1[0].laplacian = In[0].laplacian;
			data1[0].orientation = In[0].orientation;
			data2[0].x = In[0].x;
			data2[0].y = In[0].y;
			data2[0].scale = In[0].scale;
			data2[0].laplacian = In[0].laplacian;
			data2[0].orientation = In[0].orientation;
		}
	};
	Desc = splitjoin(data1){
		int i,j;
		double  cx = -0.5, cy = 0.0; //Subregion centers for the 4x4 gaussian weighting
		split duplicate();
		for(i=-8;i<12;i += 9){
			i = i-4;
			cx += 1.0;
			cy = -0.5;
			for(j=-8;j<12;j += 9){
				cy += 1.f;
				j = j -4;
				add getDesc(i,j,cx,cy);
			}
		}
		join roundrobin(5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5); //前4个为desc信息 第5个是len
	};
	Out = getDescriptor(data2,Desc){
		work
		{
			int i,j=0;
			float len=0.0;
			Out[j++].x = data2[0].x;
			Out[j++].x = data2[0].y;
			Out[j++].x = data2[0].scale;
			Out[j++].x = data2[0].laplacian;
			Out[j++].x = data2[0].orientation;
			if(data2[0].x != -1){
				for(i=0;i<80;i++)
					if((i+1)%5 == 0)
						len += Desc[i].x;
				len = sqrt(len);
				for(i=0;i<80;i++)
					if((i+1)%5 != 0)
						Out[j++].x = Desc[i].x / len;
			} else {
				for(i=0;i<80;i++)
					if((i+1)%5 != 0)
						Out[j++].x = -5;
			}
		}
		window
		{
			data2 sliding(1,1);
			Desc sliding(80,80);
			Out tumbling(69);
		}
	};
}


composite getExtremum(output stream<int x>Out,input stream<int x>In)
{
	param
		int t,int m,int b;
	
	Out = getExtremum(In){
		work
		{
			int flag = 1,scale,index;
			int r=In[0].x / R_w[t];
			int c=In[0].x % R_w[t];
			int rr,cc;
			double candidate,res;
			int layerBorder = (R_filter[t] + 1) / (2 * R_step[t]);
			if (r <= layerBorder || r >= R_h[t] - layerBorder || c <= layerBorder || c >= R_w[t] - layerBorder)
				flag = 0;
			if(flag==1){
				scale = (int)(R_w[m] / R_w[t]);
				index = (scale * r) * R_w[m] + (scale * c);
				GETRES(m,index,0,candidate); // 第3参数为类型值 0表示获得res值 1获得lap值
				if(candidate < thresh)
					flag = 0;
			}
			if(flag==1){
				for(rr=-1;rr<=1;++rr){
					for(cc=-1;cc<=1;++cc){
						index = (r+rr)*R_w[t] + (c+cc);
						GETRES(t,index,0,res);
						if(res >= candidate){
							flag = 0;
							break;
						}
						scale = (int)(R_w[m] / R_w[t]);
						index = (scale * (rr+r)) * R_w[m] + (scale * (c+cc));
						GETRES(m,index,0,res);
						if((rr != 0 || cc != 0) && res >= candidate){
							flag = 0;
							break;
						}
						scale = (int)(R_w[b] / R_w[t]);
						index = (scale * (rr+r)) * R_w[b] + (scale * (c+cc));
						GETRES(b,index,0,res);
						if(res >= candidate){
							flag=0;
							break;
						}
					}
					if(flag == 0)
						break;
				}
			}
			Out[0].x = In[0].x;
			Out[1].x = flag;
		}
		window
		{
			In sliding(1,1);
			Out tumbling(2);
		}
	};
}

composite interpolateStep(output stream<double x,double y,double scale,double laplacian>Out,input stream<int x>In)
{ //输入为flag信息和点坐标信息  每次输入2个数据
	param
		int b, int m, int t;
	Out = interpolateStepx(In){
		work
		{
			int r_off[19] = {0,0,1,-1,0,0,0,1,1,-1,-1,0,0,0,0,1,-1,1,-1};
			int c_off[19] = {1,-1,0,0,0,0,0,1,-1,1,-1,1,-1,1,-1,0,0,0,0};
			double rs[19];
			int src[19];
			double dx, dy, ds;
			double v, dxx, dyy, dss, dxy, dxs, dys;
			double rank;
			double H[3][3];
			double xi,xr,xc;
			double lap=0.0;
			int scale,index,r,c,i,tmp_s,tmp_w;
			int flag;
			int t_width = R_w[t];
			int filterStep = (R_filter[m] - R_filter[b]);
			int t_step = R_step[t];
			flag = In[1].x;
			
			if(flag == 1){
				r = In[0].x / t_width;
				c = In[0].x % t_width;
				src[0]=m;src[1]=m;src[2]=m;src[3]=m;src[4]=t;src[5]=b;
				src[6]=m;src[7]=m;src[8]=m;src[9]=m;src[10]=m;src[11]=t;
				src[12]=t;src[13]=b;src[14]=b;src[15]=t;src[16]=t;
				src[17]=b;src[18]=b;
				
				for(i=0;i<19;i++){
					tmp_w = R_w[src[i]];
					tmp_s = (int)(tmp_w / R_w[t]);
					index = tmp_s*(r + r_off[i]) * tmp_w + (tmp_s * (c + c_off[i]));
					GETRES(src[i],index,0,rs[i]);
				}
				dx = (rs[0] - rs[1]) / 2.0;
				dy = (rs[2] - rs[3]) / 2.0;
				ds = (rs[4] - rs[5]) /2.0;
				v = rs[6];
				dxx = rs[0] + rs[1] - 2*v;
				dyy = rs[2] + rs[3] - 2*v;
				dss = rs[4] + rs[5] - 2*v;
				dxy = ( rs[7] - rs[8] -
					    rs[9] + rs[10] ) / 4.0;
				dxs = ( rs[11] - rs[12] -
					    rs[13] + rs[14] ) / 4.0;
				dys = ( rs[15] - rs[16] -
						rs[17] + rs[18] ) / 4.0;	
				rank = dxx*dyy*dss + dxy*dys*dxs*2 -dxx*dys*dys - dss*dxy*dxy - dyy*dxs*dxs;
				//rank = (rank > 0 ? rank : rank*(-1.0));
				H[0][0] = (dyy*dss-dys*dys)/rank;  H[1][1] = (dxx*dss - dxs*dxs) /rank; H[2][2] = (dxx*dyy - dxy*dxy) /rank;
				H[0][1] = (-1) * (dxy*dss-dxs*dys) / rank;H[1][0] = (-1) * (dxy*dss-dxs*dys) / rank;  
				H[0][2] = (dxy*dys - dyy*dxs) / rank;H[2][0] = (dxy*dys - dyy*dxs) / rank;
				H[1][2] = (-1) * (dxx*dys - dxs*dxy) /rank;H[2][1] = (-1) * (dxx*dys - dxs*dxy) /rank;
				
				xi = H[2][0] * dx + H[2][1] * dy +H[2][2] *ds;
				xr = H[1][0] * dx + H[1][1] * dy +H[1][2] *ds;
				xc = H[0][0] * dx + H[0][1] * dy +H[0][2] *ds;
				xi = xi * (-1);
				xr = xr * (-1);
				xc = xc * (-1);

				if( fabs(xi)  >= 0.5  ||   fabs(xr)  >= 0.5  ||   fabs(xc) >= 0.5 ){
					flag = 0;
				}
			}
			
			if(flag==1){
				scale = R_w[m] / R_w[t];
				index = (scale * r) * R_w[m] + (scale * c);
				GETRES(m,index,1,lap);
				Out[0].x = (c + xc) * t_step;
				Out[0].y = (r + xr) * t_step;
				Out[0].scale = (0.1333f) * (R_filter[m] + xi * filterStep);
				Out[0].laplacian = lap;
			}else{
				Out[0].x = -1.0;
				Out[0].y = -2.0;
				Out[0].scale = -3.0;
				Out[0].laplacian = -4.0;
			}
		}
		window
		{
			In sliding(2,2);
			Out tumbling(1);
		}
	};
}

composite getIptCore(output stream<double x>Out,input stream<int x>In)
{
	param
		int t;
	int m=t-1;
	int b;
	if(t!=2 && t%2==0)
		b = t-3;
	else
		b = t-2;
	
	Out = pipeline(In){
		add getExtremum(t,m,b); // 输出值（2） 为点坐标信息  和flag值
		add interpolateStep(t,m,b);
		add getOrientation();
		add getDescriptor();
	};
}
composite getIpoint(output stream<double x>Out,input stream<int x>In)
{
	param
		int i;
	int j=2*i+2;
	Out = splitjoin(In){
		split duplicate();
		add getIptCore(j);
		add getIptCore(j+1);
		join roundrobin(69,69);	
	};
}
composite dataExpand(output stream<int x>Out, input stream<int x>In)
{
	param
		int expand;
	Out = dataExpand(In){
		work
		{
			int i=0;
			for(i=0;i<expand;i++){
				Out[i].x = In[0].x * expand + i;
			}
		}
		window
		{
			In sliding(1,1);
			Out tumbling(expand);
		}
	};
}
composite SourceExpand(output stream<double x>Out, input stream<int x>In)
{
	int expand[5] = {256,64,16,4,1};
	Out = splitjoin(In)
	{
		int i;
		split duplicate();
		for(i=0;i<5;i++)
		{
			add pipeline{
				add dataExpand(expand[i]);
				add getIpoint(i);
			};
		}
		join roundrobin(256*69,64*69,16*69,4*69,1*69);
	};
}

composite Main()
{
	stream<int x> Source;
	stream<double x> Ipts;
	//Ipts 每组成员 含有69个数据

	Source = getSource(){
		int x=0;
		work
		{
			Source[0].x = x;
			x++;
		}
	};
	Ipts = SourceExpand(Source)();
	Sink(Ipts)
	{
		work
		{
			int i;
			for(i=0;i<69;i++){
				if(Ipts[0].x == -1)
					break;
				ipts_info[ipts_nums++] = (float)Ipts[i].x;
			}
		}
		window
		{
			Ipts sliding(69,69);
		}
	};
}
