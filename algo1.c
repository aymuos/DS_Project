#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#define LOGO 4
#define VAL 255

struct logo_dimension
{
	int row, col;
};
typedef struct logo_dimension L_DIM;
L_DIM l_dim[LOGO + 1];

int ***rgb_logo, **rgb_image, strategy;
char logos[LOGO][25], headers[LOGO + 1][30];

void create_matrix(char envelope[]);
FILE *save_header(char file[], char hdr[], int j);
int check_existence(char image[]);
void two_cross_2_bidisha();
void get_element_bidisha(int src[], int two_crs2[], int sc, int img[], int i, int j);
void insert_bit_bidisha(int bits[], int ref[], int rf_sprt[], int seg_cnt, int i, int j, int two_crs2[], int sign[]);
int generate_bit(int bits[], int bytes[][9], int u, int j);
void create_stego();
void unpack_bidisha(char stego[]);
void check_tolerance(int u);
void check_consistency();
void distortion(char envelope[]);

void create_matrix(char envelope[])
{
	char logo[25], hdr[25], container[25], c;
	int i, val, logo_count, j, k;
	FILE *fs;

	rgb_logo = (int ***)malloc(LOGO * sizeof(int **));
	for (j = 0; j < LOGO; j++)
	{
		strcpy(logo, logos[j]); //logo has old logo img names , hdr has new logo image
		strcpy(hdr, headers[j]);
		fs = save_header(logo, hdr, j); //SAVING HEADERS FOR LOGOS

		rgb_logo[j] = (int **)malloc(l_dim[j].row * sizeof(int *));
		for (i = 0; i < l_dim[j].row; i++)
		{
			rgb_logo[j][i] = (int *)malloc(3 * l_dim[j].col * sizeof(int)); //3 cuz 1 pixel= 3 bytes @sm
			for (k = 0; k < 3 * l_dim[j].col; k++)
			{
				fscanf(fs, "%d", &rgb_logo[j][i][k]);
				fscanf(fs, "%c", &c);
			}
		}
		fclose(fs);
		/* printf("\n Logo elements are:- \n");
 for(i=0;i<30;i+=6)
   printf("R=%d,G=%d,B=%d. R=%d,G=%d,B=%d. \n",rgb_logo[j][0][i],rgb_logo[j][0][i+1],rgb_logo[j][0][i+2],rgb_logo[j][0][i+3],rgb_logo[j][0][i+4],rgb_logo[j][0][i+5]);
 for(i=210;i<80*3;i+=6)
  printf("R=%d,G=%d,B=%d. R=%d,G=%d,B=%d. \n",rgb_logo[j][79][i],rgb_logo[j][79][i+1],rgb_logo[j][79][i+2],rgb_logo[j][79][i+3],rgb_logo[j][79][i+4],rgb_logo[j][79][i+5]);*/
	}

	strcpy(hdr, headers[j]);
	fs = save_header(envelope, hdr, j); // SAVING HEADERS FOR CONTAINER

	rgb_image = (int **)malloc(l_dim[j].row * sizeof(int *));
	for (i = 0; i < l_dim[j].row; i++)
	{
		rgb_image[i] = (int *)malloc(sizeof(int) * l_dim[j].col * 3);
		for (k = 0; k < 3 * l_dim[j].col; k++)
		{
			fscanf(fs, "%d", &rgb_image[i][k]);
			fscanf(fs, "%c", &c);
		}
	}
	fclose(fs);
	/* printf("\n Container elements are:- \n");
 for(i=0;i<30;i+=6)
   printf("R=%d,G=%d,B=%d. R=%d,G=%d,B=%d. \n",rgb_image[0][i],rgb_image[0][i+1],rgb_image[0][i+2],rgb_image[0][i+3],rgb_image[0][i+4],rgb_image[0][i+5]);
 for(i=1506;i<512*3;i+=6)
  printf("R=%d,G=%d,B=%d. R=%d,G=%d,B=%d. \n",rgb_image[511][i],rgb_image[511][i+1],rgb_image[511][i+2],rgb_image[511][i+3],rgb_image[511][i+4],rgb_image[511][i+5]);*/

	/*
 printf("\n Dimensions:- \n ");
 for(i=0;i<5;i++)
   printf("row=%d, col=%d \n ",l_dim[i].row,l_dim[i].col);*/
}

FILE *save_header(char file[], char hdr[], int j) //file->LOGO OR CONTAINER FILE NAME, hdr->HEADER FILE FOR CORRESPONDING 								//LOGO OR CONTAINER FILE
{
	FILE *fs, *fd;
	char buf[100];
	int nw_lne_count = 0, i = 0;

	fs = fopen(file, "r+");
	if (fs == NULL)
	{
		printf("\n Logo or container image opening error. ");
		exit(0);
	}
	while (nw_lne_count < 2) //reading 1st line of old logo file @sm
	{
		fscanf(fs, "%c", &buf[i++]);
		if (buf[i - 1] == '\n')
			nw_lne_count++;
	}

	l_dim[j].row = 0;
	while (1)
	{
		fscanf(fs, "%c", &buf[i++]); //READING THE NO/ OF PIXELS IN IMAGE MATRIX
		if (buf[i - 1] == ' ')
			break;
		l_dim[j].row = 10 * l_dim[j].row + (buf[i - 1] - 48);
	}

	l_dim[j].col = 0;
	while (1)
	{
		fscanf(fs, "%c", &buf[i++]); //READING THE NO. OF COLUMNS OF PIXELS IN IMAGE MATRIX
		if (buf[i - 1] == '\n')
			break;
		l_dim[j].col = 10 * l_dim[j].col + (buf[i - 1] - 48);
	}

	fscanf(fs, "%c", &buf[i++]); //READING LAST HEADER LINE(MAXIMUM INTENSITY)
	fscanf(fs, "%c", &buf[i++]);
	fscanf(fs, "%c", &buf[i++]);
	fscanf(fs, "%c", &buf[i++]);
	buf[i] = '\0'; //SETTING NULL IN END OF HEADER STRING

	fd = fopen(hdr, "w+");
	if (fd == NULL)
	{
		printf("\n logo_hdr.txt couldn't be opened. ");
		exit(0);
	}
	fputs(buf, fd); //copying the old header image to new image fd @sm

	fclose(fd);
	return fs;

} //Save Header Function Ends

void main()
{
	char envelope[25], stego[30];
	int i, choice, u, attempt;

	printf("\n Give the names of the logo images:- \n");
	attempt = 0;
l1:
	scanf("%s", logos[0]);
	// strcpy(logos[0],"l_map.ppm");
	if (check_existence(logos[0]) == 0) //checks if the img is present in dir or not
	{
		if (attempt == 4)
			exit(0);
		attempt++;
		goto l1;
	}

	attempt = 0;
l2:
	scanf("%s", logos[1]);
	// strcpy(logos[1],"l_sea.ppm");
	if (check_existence(logos[1]) == 0)
	{
		if (attempt == 4)
			exit(0);
		attempt++;
		goto l2;
	}

	attempt = 0;
l3:
	scanf("%s", logos[2]);
	// strcpy(logos[2],"l_sky.ppm");
	if (check_existence(logos[2]) == 0)
	{
		if (attempt == 4)
			exit(0);
		attempt++;
		goto l3;
	}

	attempt = 0;
l4:
	scanf("%s", logos[3]);
	// strcpy(logos[3],"l_globe.ppm");
	if (check_existence(logos[3]) == 0)
	{
		if (attempt == 4)
			exit(0);
		attempt++;
		goto l3;
	}

	printf("\n Enter the name of the container image:- ");
	attempt = 0;
l5:
	scanf("%s", envelope);
	// strcpy(envelope,"ice.ppm");
	if (check_existence(envelope) == 0)
	{
		if (attempt == 4)
			exit(0);
		attempt++;
		goto l4;
	}
	// inputing of images completed

	//CREATING NAMES FOR LOGO HEADERS
	for (i = 0; i < LOGO; i++)
	{
		strcpy(headers[i], logos[i]);
		headers[i][strlen(headers[i]) - 4] = '\0';
		strcat(headers[i], "_hdr.ppm");
	}
	strcpy(headers[i], envelope); //CREATING NAMES FOR LOGO HEADERS
	headers[i][strlen(headers[i]) - 4] = '\0';
	strcat(headers[i], "_hdr.ppm");

	create_matrix(envelope); //data of container image is copied to global var rgb_logo  @sm
	check_consistency();  //checking if total pixel of container image is greater than the sum of the pixels of the logo image

	printf("\n Choose a method for inserting logo image in the container image. ");
	printf("\n Press 1: For inserting 4 bits of 4 different logo images in 1 2*2 matrix sequentially. ");
	printf("\n Press 2: For inserting 4 bits of 1 logo image in 2*2 matrix sequentially. ");
	printf("\n Press 3: For inserting 4 bits of 4 different logo images in 4 equally divided segemnts of container image. ");
	printf("\n\n Enter your choice:- ");
	scanf("%d", &strategy);
	getchar();

	printf("\n Please read the instructions below. \n");
	printf("\n Press 1: To create the digital watermarked image. ");
	printf("\n Press 2: To check tolerance. ");
a:
	printf("\n\n Please provide your choice:- ");
	scanf("%d", &choice);
	getchar();

	switch (choice)
	{
	case 1:
		two_cross_2_bidisha();
		create_stego();
		printf("\n Digitally watermarked image creation is done. ");
		attempt = 0;
		printf("\n Enter the watermarked image name to be extracted:- ");
	l6:
		gets(stego);
		if (check_existence(stego) == 0)
		{
			if (attempt == 4)
			{
				printf("\n You have violated program consistence. Terminating........");
				exit(0);
			}
			printf("\n Requested watermarked image doesn't found. Provide a  correct name:- ");
			attempt++;
			goto l6;
		}
		unpack_bidisha(stego);
		distortion(envelope);
		break;
	case 2:
		printf("\n Enter the value of tolerance:- ");
		scanf("%d", &u);
		check_tolerance(u);
		break;
	default:
		printf("\n Wrong choice. Please press '1' or '2'. \n");
		goto a;
	}
}

void check_consistency()
{
	//LOGO : 4
	/*
	
		1 px = 3 byte (3 int)
	*/

	int i, s = 0;
	for (i = 0; i < LOGO; i++){
		s += l_dim[i].row * l_dim[i].col;
		//
		printf("%d \t ,%d \t , %d \n", l_dim[i].row,l_dim[i].col,s);
	//
	}
	//change from s*24 to s*3
	if (s * 24 >= l_dim[LOGO].row * l_dim[LOGO].col * 3)
	{
		//hehe printing @sm
		printf("%d \t ,%d \t , %d", l_dim[LOGO].row,l_dim[LOGO].col,s);

		//
		printf("\n Container image size is insufficient to hold this many logo images. Terminating.......... \n");
		exit(0);
	}
}

void distortion(char envelope[])
{
	int i, j, k, c = 0, ds[2], t, u, max = 0;
	float per, dis[VAL];
	FILE *fs, *fd, *fl;

	for (i = 0; i < VAL; i++)
		dis[i] = 0.0;

	fd = fopen("Distortion.txt", "w+");
	if (fd == NULL)
	{
		printf("\n Distortion calculation file opening error. Trying again. Terminating........ \n");
		exit(0);
	}

	fs = fopen(envelope, "r+");
	if (fs == NULL)
	{
		printf("\n Distorted stego file opening error. Terminating........ \n");
		exit(0);
	}

	while (c < 4)
	{
		if (fgetc(fs) == '\n')
			c++;
	}

	fprintf(fd, "%s %s %s %s", " Distortion Information of Steganographed Image ", envelope, ":- ", "\n\n");
	for (i = 0; i < l_dim[LOGO].row; i++)
	{
		for (j = 0; j < l_dim[LOGO].col * 3; j++)
		{
			fscanf(fs, "%d", &t);
			t = t - rgb_image[i][j];
			if (t < 0)
				t *= (-1);

			dis[t] = dis[t] + 1;
			if (t > max)
			{
				max = t;
				ds[0] = i;
				ds[1] = j;
			}
		}
	}
	fclose(fs);

	for (i = 0; i <= max; i++)
	{
		per = (dis[i] * 100) / (l_dim[LOGO].row * l_dim[LOGO].col * 3);
		fprintf(fd, "%s %d %s %.0f %s %.3f %s", " Total difference count for ", i, " => ", dis[i], ";  Percentage = ", per, "% \n");
	}
	// printf("\n Highest distortion info for container:- val=%d, i=%d, j=%d. \n",max,ds[0],ds[1]);

	for (k = 0; k < LOGO; k++)
	{
		for (i = 0; i < VAL; i++)
			dis[i] = 0.0;
		fs = fopen(headers[k], "r+");
		if (fs == NULL)
		{
			printf("\n Distorted logo  file opening error. Terminating........ \n");
			exit(0);
		}

		fl = fopen(logos[k], "r+");
		c = 0;
		while (c < 4)
		{
			fgetc(fs);
			if (fgetc(fl) == '\n')
				c++;
		}

		max = 0;
		fprintf(fd, "%s %s %s", "\n\n	Distortion information of ", headers[k], " :- \n\n");
		for (i = 0; i < l_dim[k].row; i++)
		{
			for (j = 0; j < l_dim[k].col * 3; j++)
			{
				fscanf(fs, "%d", &t);
				fscanf(fl, "%d", &u);
				t = u - t;
				if (t < 0)
					t *= (-1);

				dis[t] = dis[t] + 1;
				if (t > max)
				{
					max = t;
					ds[0] = i;
					ds[1] = j;
				}
			}
		}
		fclose(fs);
		fclose(fl);

		//	if(k==0)
		//	  printf("\n Highest distortion-> val=%d, row=%d, col=%d. \n",max,ds[0],ds[1]);

		for (i = 0; i <= max; i++)
		{
			per = (dis[i] * 100) / (l_dim[k].row * l_dim[k].col * 3);
			fprintf(fd, "%s %d %s %.0f %s %.3f %s", " Total difference count for ", i, " => ", dis[i], ";  Percentage = ", per, "% \n");
		}
		//	printf("\n Highest distortion info of %s :- val=%d, i=%d, j=%d. \n",headers[k],max,ds[0],ds[1]);
	}
}

int check_existence(char image[])
{
	struct dirent *dr;
	DIR *dir;
	dir = opendir(".");

	if (dir)
	{
		while ((dr = readdir(dir)) != NULL)
		{
			if (strcmp(dr->d_name, image) == 0)
				return 1;
		}

		if (dr == NULL) //Check if requested image found or not
		{
			printf("\n Requested logo image doesn't exist. Try again. \n");
			return 0;
		}
		closedir(dir);

	} // if(dir)
}

void two_cross_2_bidisha()
{
	int i, j, k, t, s, m, a, b, c, d, pass = 1, ret, sc, seg_cnt = 0;
	int src[4], img[4], two_crs2[4], rf_sprt[5], ref[4], bit[4], bits[4], sign[2];
	int ul, ll, l, dif, cnt1 = 0, cnt2 = 0, bytes[4][9], bts[4], quit = 1;

	for (i = 0; i < l_dim[LOGO].row; i += 2) //CREATING CAHNGE IN 2*2 MATRIX
	{
		for (j = 0; j < l_dim[LOGO].col * 3; j += 2)
		{
			s = 0;
			sc = seg_cnt;
			cnt1 = cnt2 = 0;
			get_element_bidisha(src, two_crs2, sc, img, i, j);

			//		if(i==182 && j==834)
			//		   printf("\n img[0]=%d, img[1]=%d, img[2]=%d, img[3]=%d. ",img[0],img[1],img[2],img[3]);

			//STARTING CREATION OF CONVERTED MATRIX
		re:
			sign[0] = sign[1] = 0;

			while (sc > 9) //STARTING CALCULATING REFERRENCE0
				sc /= 10;
			s = sc % 3;
			if (s == 0)
			{
				t = 4;
				rf_sprt[0] = 0;
			}
			if (s == 1)
			{
				t = 6;
				rf_sprt[0] = 1;
			}
			if (s == 2)
			{
				t = 8;
				rf_sprt[0] = 2;
			}

			if (img[0] <= 255)
			{
				ll = img[0];
				while (1)
				{
					if (ll % t == 0)
						break;
					ll--;
				}
				ref[0] = (ll + t / 2);
			}
			if (ref[0] == 255 && s == 1)
				ref[0] = 253;
			bit[0] = ((ll % 100) / 10) % 2;

			if (cnt1 > 0 || cnt2 > 0)
				goto mv;

			sc = seg_cnt; //STARTING CALCULATING REFERRENCE1
			while (sc > 99)
				sc /= 10;
			if (sc < 10)
				s = sc % 3;
			else
				s = ((sc % 10) * 10 + ((sc % 100) / 10)) % 3;

			ll = (img[1] / 10) * 10;
			ul = ll + 1;
			if (s == 0)
				while (1)
				{
					if (ul % 4 == 0)
						break;
					ul++;
				}

			if (s == 1)
				while (1)
				{
					if (ul % 6 == 0)
						break;
					ul++;
				}

			if (s == 2)
				while (1)
				{
					if (ul % 7 == 0)
						break;
					ul++;
				}

			rf_sprt[1] = ul - ll;
			ref[1] = (ll + ul + 1) / 2;
			bit[1] = ((ul % 100) / 10) % 2;

			if (img[2] < 250) //STARTING CALCULATING REFERRENCE2
			{
				sc = seg_cnt;
				while (sc > 99)
					sc /= 10;
				s = (sc % 10 + sc / 10) % 3;
				t = (img[2] / 10) * 10 + 3;
				rf_sprt[2] = 3;

				if (s == 1)
				{
					t += 2;
					rf_sprt[2] = 5;
				}

				if (s == 2)
				{
					t += 4;
					rf_sprt[2] = 7;
				}

				sc /= 2;
				if (sc % 2 == 0)
				{
					ll = t - 1;
					while (1)
					{
						if (ll % 4 == 0)
							break;
						ll--;
					}
					ref[2] = (ll + t + 1) / 2;
				}

				if (sc % 2 == 1)
				{
					ul = t + 1;
					while (1)
					{
						if (ul % 4 == 0)
							break;
						ul++;
					}
					ref[2] = (t + ul + 1) / 2;
				}
			}
			else if (img[2] >= 250)
			{
				ref[2] = 252;
				rf_sprt[2] = 3;
			}
			bit[2] = ref[2] % 2;

			sc = seg_cnt;
			while (sc > 999)
				sc /= 10;
			s = (sc % 10 + (sc % 100) / 10 + sc / 100) % 3;

			if (s == 0)
			{
				if (img[3] < 11)
				{
					ll = 0;
					ul = 10;
				}
				else if (img[3] < 244)
				{
					ll = img[3];
					ul = ll + 1;
					while (1)
					{
						if (ll % 10 == (ll % 100) / 10)
							break;
						ll--;
					}
					while (1)
					{
						if (ul % 10 == (ul % 100) / 10)
							break;
						ul++;
					}

					if (ul % 100 == 0) //FOR CASES LIKE ll=199 OR 99 & ul=100 OR 200
						ul = ul + 11;
				}
				else
				{
					ll = 244;
					ul = 255;
				}
				rf_sprt[3] = 0;
				rf_sprt[4] = 0;
			}

			if (s == 1)
			{
				if (img[3] < 252)
				{
					ll = img[3];
					while (1)
					{
						if (ll % 9 == 0)
							break;
						ll--;
					}
					ul = ll + 9;
				}
				else
				{
					ll = 252;
					ul = 255;
				}
				rf_sprt[3] = 1;
				rf_sprt[4] = 0;
			}

			if (s == 2)
			{
				ll = (img[3] / 10) * 10;
				ul = ll + 1;
				while (1)
				{
					t = sqrt(ul);
					for (s = 2; s <= t; s++)
						if (ul % s == 0)
							break;
					if (s > t)
						break;
					ul++;
				}
				rf_sprt[3] = 2;
				rf_sprt[4] = ul - ll;
			}
			ref[3] = (ll + ul + 1) / 2;
			sc = seg_cnt;
			while (sc > 9)
				sc /= 10;
			bit[3] = (sc + ((ref[3] % 100) / 10)) % 2;

			//		if(i==182 && j==834)
			//		  printf("\n rs0=%d, rs1=%d, rs2=%d, rs3=%d, rs4=%d. ",rf_sprt[0],rf_sprt[1],rf_sprt[2],rf_sprt[3],rf_sprt[4]);

			if (cnt1 == 0 && cnt2 == 0)
				if (generate_bit(bits, bytes, i, j) == 0) //CREATING BIT SET FOR INSERTION
					quit = 0;
			//		if(i==182 && j==834)
			//		  printf("\n Before encryption-  bit0=%d, bit1=%d, bit2=%d, bit3=%d. ",bits[0],bits[1],bits[2],bits[3]);

		mv:
			bts[0] = bits[0] ^ bit[0];
			bts[1] = bits[1] ^ bit[1] ^ 1; //CREATING ENCRYPTED BIT SET
			bts[2] = bits[2] ^ bit[2];
			bts[3] = bits[3] ^ bit[3] ^ 1;

			//		if(i==182 && j==834)
			//		  printf("\n After encryption-  bit0=%d, bit1=%d, bit2=%d, bit3=%d. ",bts[0],bts[1],bts[2],bts[3]);

			insert_bit_bidisha(bts, ref, rf_sprt, seg_cnt, i, j, two_crs2, sign);
			if (sign[0] < 0 && cnt1 < 5)
			{
				img[0] = img[0] - sign[0];
				if (sign[0] > -4)
					img[0] += 2;
				cnt1++;
				goto re;
			}

			if (sign[1] > 0 && cnt2 < 5)
			{
				img[0] = img[0] - sign[1];
				if (sign[0] < 4)
					img[0] -= 2;
				cnt2++;
				goto re;
			}

			seg_cnt++;
			if (quit == 0)
			{ //	printf("\n seg_cnt=%d. ",seg_cnt);
				return;
			}
		} //for(j=0;j<l_dim[LOGO].col;j+=2)
	}	  // for(i=0;i<l_dim[LOGO].row;i+=2)
}

void get_element_bidisha(int src[], int two_crs2[], int sc, int img[], int i, int j)
{
	int m, s = 0, row_mv = 1, col_mv = 1, div = 1, a, b, c, d;

	if (strategy == 3)
	{
		row_mv = l_dim[LOGO].row / 2;
		col_mv = (l_dim[LOGO].col / 2) * 3;
		div = 2;
	}
	m = sc % 1000;
	while (m > 0)
	{
		s += m % 10;
		m /= 10;
	}
	m = s % 4;

	switch (m) //CHOOSING INDEX SEQUENCE FOR BIT INSERTION IN 2*2 MATRIX
	{
	case 0:
		src[0] = rgb_image[i / div][j / div];
		src[1] = rgb_image[i / div][j / div + col_mv];
		src[2] = rgb_image[i / div + row_mv][j / div];
		src[3] = rgb_image[i / div + row_mv][j / div + col_mv];
		break;
	case 1:
		src[0] = rgb_image[i / div][j / div + col_mv];
		src[1] = rgb_image[i / div + row_mv][j / div];
		src[2] = rgb_image[i / div + row_mv][j / div + col_mv];
		src[3] = rgb_image[i / div][j / div];
		break;
	case 2:
		src[0] = rgb_image[i / div + row_mv][j / div];
		src[1] = rgb_image[i / div + row_mv][j / div + col_mv];
		src[2] = rgb_image[i / div][j / div];
		src[3] = rgb_image[i / div][j / div + col_mv];
		break;
	case 3:
		src[0] = rgb_image[i / div + row_mv][j / div + col_mv];
		src[1] = rgb_image[i / div][j / div];
		src[2] = rgb_image[i / div][j / div + col_mv];
		src[3] = rgb_image[i / div + row_mv][j / div];
		break;

	default:
		printf("\n Error introduced in switch-case of two_cross_2_disha(). ");
	}

	//		if(i==182 && j==834)
	//		   printf("\n j=%d, sc=%d, a=%d, b=%d, c=%d, d=%d. ",j,sc,src[0],src[1],src[2],src[3]);

	two_crs2[0] = img[0] = (src[0] + src[1] + src[2] + src[3] + 4 - 1) / 4; //APPLYING 1st CONVERSION ON ELEMENT1

	if ((src[0] - src[1] + src[2] - src[3]) < 0)
	{
		two_crs2[1] = img[1] = (src[0] - src[1] + src[2] - src[3] - 4 + 1) / 4;
		img[1] *= (-1);
	}
	else
		two_crs2[1] = img[1] = (src[0] - src[1] + src[2] - src[3] + 4 - 1) / 4;

	if ((src[0] + src[1] - src[2] - src[3]) < 0)
	{
		two_crs2[2] = img[2] = (src[0] + src[1] - src[2] - src[3] - 4 + 1) / 4;
		img[2] *= (-1);
	}
	else
		two_crs2[2] = img[2] = (src[0] + src[1] - src[2] - src[3] + 4 - 1) / 4;

	if ((src[0] - src[1] - src[2] + src[3]) < 0)
	{
		two_crs2[3] = img[3] = (src[0] - src[1] - src[2] + src[3] - 4 + 1) / 4;
		img[3] *= (-1);
	}
	else
		two_crs2[3] = img[3] = (src[0] - src[1] - src[2] + src[3] + 4 - 1) / 4;
}

void insert_bit_bidisha(int bits[], int r[], int rf_sprt[], int seg_cnt, int i, int j, int two_crs2[], int sign[])
{
	int a, b, c, d, div = 1, row_mv = 1, col_mv = 1, ve[2], p = 0, k, m, q, s = 0, ref[4];

	ref[0] = r[0];
	ref[1] = r[1];
	ref[2] = r[2];
	ref[3] = r[3];
	// if(i==182 && j==834)
	//  printf("\n i=%d, ref[0]=%d, ref[1]=%d, ref[2]=%d, ref[3]=%d. ",i,ref[0],ref[1],ref[2],ref[3]);

	if (rf_sprt[0] < 2) //INSERTING BITS[0]
	{
		if (bits[0] == 0)
			ref[0] -= 1;
		else
			ref[0] += 1;
	}
	else if (rf_sprt[0] == 2)
	{
		if (bits[0] == 0)
			ref[0] -= 2;
		else
			ref[0] += 2;
	}

	if (rf_sprt[1] < 6) //INSERTING BITS[1]
	{
		if (bits[1] == 0)
			ref[1] -= 1;
		else
			ref[1] += 1;
	}
	else if (rf_sprt[1] > 5)
	{
		if (bits[1] == 0)
			ref[1] -= 2;
		else
			ref[1] += 2;
	}

	if (rf_sprt[2] == 3) //INSERTING BITS[2]
	{
		if (bits[2] == 0)
			ref[2] -= 1;
		else
			ref[2] += 2;
	}
	else if (rf_sprt[2] == 5)
	{
		if (bits[2] == 0)
			ref[2] -= 2;
		else
			ref[2] += 2;
	}
	else if (rf_sprt[2] == 7)
		if (bits[2] == 0)
			ref[2] -= 2;

	if (rf_sprt[3] < 2)
	{
		if (bits[3] == 0)
			ref[3] -= 2;
		else
			ref[3] += 2;
	}
	else if (rf_sprt[3] == 2)
	{
		if (rf_sprt[3] <= 2)
		{
			if (bits[3] == 0)
				ref[3] -= 1;
			else
				ref[3] = ref[3];
		}
		else if (rf_sprt[3] <= 5)
		{
			if (bits[3] == 0)
				ref[3] -= 1;
			else
				ref[3] += 1;
		}
		else
		{
			if (bits[3] == 0)
				ref[3] -= 2;
			else
				ref[3] += 2;
		}
	}

	// if(i==182 && j==834)
	//  printf("\n j=%d, ref[0]=%d, ref[1]=%d, ref[2]=%d, ref[3]=%d. ",j,ref[0],ref[1],ref[2],ref[3]);

	for (k = 1; k < 4; k++)
		if (two_crs2[k] < 0)
			ref[k] *= (-1);

	a = ref[0];
	b = ref[1];
	c = ref[2];
	d = ref[3];

	ref[0] = a + b + c + d; //APPLYING REVERSE FORMULA
	ref[1] = a - b + c - d;
	ref[2] = a + b - c - d;
	ref[3] = a - b - c + d; //STEGO VALUES ARE READY TO BE INSERTED

	if (ref[1] < 0)
		ve[p++] = ref[1];

	if (ref[2] < 0)
		ve[p++] = ref[2];

	if (ref[3] < 0)
		ve[p++] = ref[3];

	for (k = 0; k < p - 1; k++)
	{
		for (q = 0; q < k - p - 1; q++)
			if (ve[q] > ve[q + 1])
			{
				m = ve[q];
				ve[q] = ve[q + 1];
				ve[q + 1] = m;
			}
	}

	if (p > 0)
	{	//	if(i==182 && j==834)
		//	  printf("\n To -ve write- ve[0]=%d, ref[0]=%d, ref[1]=%d, ref[2]=%d, ref[3]=%d. ",ve[0],ref[0],ref[1],ref[2],ref[3]);
		sign[0] = ve[0];
		return;
	}

	p = 0;
	if (ref[0] > 255)
		ve[p++] = ref[0];

	if (ref[1] > 255)
		ve[p++] = ref[1];

	if (ref[2] > 255)
		ve[p++] = ref[2];

	if (ref[3] > 255)
		ve[p++] = ref[3];

	for (k = 0; k < p - 1; k++)
	{
		for (q = 0; q < k - p - 1; q++)
			if (ve[q] > ve[q + 1])
			{
				m = ve[q];
				ve[q] = ve[q + 1];
				ve[q + 1] = m;
			}
	}

	if (p > 0)
	{ //	if(i==182 && j==834)
		//  printf("\n To +ve write- ref[0]=%d, ref[1]=%d, ref[2]=%d, ref[3]=%d. ",ref[0],ref[1],ref[2],ref[3]);
		sign[1] = ve[p - 1] - 255;
		return;
	}

	// if(i==182 && j==834)
	//  printf("\n i=%d, j=%d, ref[0]=%d, ref[1]=%d, ref[2]=%d, ref[3]=%d. ",i,j,ref[0],ref[1],ref[2],ref[3]);

	if (strategy == 3)
	{
		div = 2;
		row_mv = l_dim[LOGO].row / 2;
		col_mv = (l_dim[LOGO].col / 2) * 3;
	}

	m = seg_cnt % 1000;
	while (m > 0)
	{
		s += m % 10;
		m /= 10;
	}
	m = s % 4;
	// printf(" i=%d, j=%d. ",i,j);

	switch (m) //WRITING STEGO VALUES IN CONTAINER MATRIX
	{
	case 0:
		rgb_image[i / div][j / div] = ref[0];
		rgb_image[i / div][j / div + col_mv] = ref[1];
		rgb_image[i / div + row_mv][j / div] = ref[2];
		rgb_image[i / div + row_mv][j / div + col_mv] = ref[3];
		break;

	case 1:
		rgb_image[i / div][j / div] = ref[3];
		rgb_image[i / div][j / div + col_mv] = ref[0];
		rgb_image[i / div + row_mv][j / div] = ref[1];
		rgb_image[i / div + row_mv][j / div + col_mv] = ref[2];
		break;

	case 2:
		rgb_image[i / div][j / div] = ref[2];
		rgb_image[i / div][j / div + col_mv] = ref[3];
		rgb_image[i / div + row_mv][j / div] = ref[0];
		rgb_image[i / div + row_mv][j / div + col_mv] = ref[1];
		break;

	case 3:
		rgb_image[i / div][j / div] = ref[1];
		rgb_image[i / div][j / div + col_mv] = ref[2];
		rgb_image[i / div + row_mv][j / div] = ref[3];
		rgb_image[i / div + row_mv][j / div + col_mv] = ref[0];
		break;

	default:
		printf("\n Error introduced in switch-case of insert_bit_disha(). ");
	}

	if (ref[0] > 255)
		printf("\n %d, ", ref[0]);

	if (ref[1] < 0 || ref[1] > 255)
		printf("%d, ", ref[1]);

	if (ref[2] < 0 || ref[2] > 255)
		printf("%d, ", ref[2]);

	if (ref[3] < 0 || ref[3] > 255)
		printf("%d, ", ref[3]);

} //int insert_bit_disha(int bits[], int ref[], int seg_cnt, int i, int j)

void unpack_bidisha(char stego[])
{
	int i, j, t, seg_cnt = 0, m, s, sc, a, b, c, d, ul, ll, dif, nl = 0, val;
	int bytes[4][8], src[4], ref[4], img[4], two_crs2[4], bit[4];
	int lgo_row13 = 0, lgo_col13 = 0, lgo_row2 = 0, lgo_col2 = 0, n = 0;
	char ch;
	FILE *l[4], *f;

	if (strategy == 1 || strategy == 3)
	{
		l[0] = fopen(headers[0], "a+");
		l[1] = fopen(headers[1], "a+");
		l[2] = fopen(headers[2], "a+");
		l[3] = fopen(headers[3], "a+");
	}
	if (strategy == 2)
		l[0] = fopen(headers[0], "a+");

	f = fopen(stego, "r+");
	if (f == NULL)
	{
		printf("\n There is some problem with the watermarked image. Terminating the program....... ");
		exit(0);
	}
	while (nl < 4)
	{
		fscanf(f, "%c", &ch);
		if (ch == '\n')
			nl++;
	}

	for (i = 0; i < l_dim[LOGO].row; i++)
		for (j = 0; j < l_dim[LOGO].col * 3; j++)
		{
			fscanf(f, "%d", &rgb_image[i][j]);
			fscanf(f, "%c", &ch);
		}

	for (i = 0; i < l_dim[LOGO].row; i += 2)
	{
		for (j = 0; j < l_dim[LOGO].col * 3; j += 2)
		{
			sc = seg_cnt;

			get_element_bidisha(src, two_crs2, sc, img, i, j);
			a = img[0];
			b = img[1];
			c = img[2];
			d = img[3];

			// if(i==182 && j==834)
			//  printf("\n j=%d, img[0]=%d, img[1]=%d, img[2]=%d, img[3]=%d. ",j,a,b,c,d);

			while (sc > 9) //STARTING CALCULATING REFERRENCE0
				sc /= 10;
			s = sc % 3;
			if (s == 0)
				t = 4;
			if (s == 1)
				t = 6;
			if (s == 2)
				t = 8;

			if (a <= 255)
			{
				ll = a;
				while (1)
				{
					if (ll % t == 0)
						break;
					ll--;
				}
				ref[0] = (ll + t / 2);
			}
			if (ref[0] == 255 && s == 1)
				ref[0] = 253;
			bit[0] = ((ll % 100) / 10) % 2;

			sc = seg_cnt; //STARTING CALCULATING REFERRENCE1
			while (sc > 99)
				sc /= 10;
			if (sc < 10) //CALCULATING REFERRENCE[1]
				s = sc % 3;
			else
				s = ((sc % 10) * 10 + ((sc % 100) / 10)) % 3;

			ll = (b / 10) * 10;
			ul = ll + 1;
			if (s == 0)
				while (1)
				{
					if (ul % 4 == 0)
						break;
					ul++;
				}

			if (s == 1)
			{
				while (1)
				{
					if (ul % 6 == 0)
						break;
					ul++;
				}
			}

			if (s == 2)
				while (1)
				{
					if (ul % 7 == 0)
						break;
					ul++;
				}

			ref[1] = (ll + ul + 1) / 2;
			bit[1] = ((ul % 100) / 10) % 2;

			if (c < 250) //CALCULATING REFERRENCE[2]
			{
				int p;
				sc = seg_cnt;
				while (sc > 99)
					sc /= 10;
				s = (sc % 10 + sc / 10) % 3;

				t = (c / 10) * 10 + 3;

				if (s == 1)
					t += 2;
				if (s == 2)
					t += 4;

				sc /= 2;
				if (sc % 2 == 0)
				{
					ll = t - 1;
					while (1)
					{
						if (ll % 4 == 0)
							break;
						ll--;
					}
					ref[2] = (ll + t + 1) / 2;
				}

				if (sc % 2 == 1)
				{
					ul = t + 1;
					while (1)
					{
						if (ul % 4 == 0)
							break;
						ul++;
					}
					ref[2] = (t + ul + 1) / 2;
				}
			}
			else if (c >= 250)
				ref[2] = 252;
			bit[2] = ref[2] % 2;

			sc = seg_cnt;
			while (sc > 999)
				sc /= 10;
			s = (sc % 10 + (sc % 100) / 10 + sc / 100) % 3;

			if (s == 0)
			{
				if (d < 11)
				{
					ll = 0;
					ul = 10;
				}
				else if (d < 244)
				{
					ll = d;
					ul = ll + 1;
					while (1)
					{
						if (ll % 10 == (ll % 100) / 10)
							break;
						ll--;
					}
					while (1)
					{
						if (ul % 10 == (ul % 100) / 10)
							break;
						ul++;
					}

					if (ul % 100 == 0) //FOR CASES LIKE ll=199 OR 99 & ul=100 OR 200
						ul = ul + 11;
				}
				else
				{
					ll = 244;
					ul = 255;
				}
			}

			if (s == 1)
			{
				if (d < 252)
				{
					ll = d;
					while (1)
					{
						if (ll % 9 == 0)
							break;
						ll--;
					}
					ul = ll + 9;
				}
				else
				{
					ll = 252;
					ul = 255;
				}
			}

			if (s == 2)
			{
				ll = (d / 10) * 10;
				ul = ll + 1;
				while (1)
				{
					t = sqrt(ul);
					for (s = 2; s <= t; s++)
						if (ul % s == 0)
							break;
					if (s > t)
						break;
					ul++;
				}
			}
			ref[3] = (ll + ul + 1) / 2;
			sc = seg_cnt;
			while (sc > 9)
				sc /= 10;
			bit[3] = (sc + ((ref[3] % 100) / 10)) % 2;

			//		 if(i==182 && j==834)
			//		  printf("\n j=%d, sc=%d, ref[0]=%d, ref[1]=%d, ref[2]=%d, ref[3]=%d. ",j,sc,ref[0],ref[1],ref[2],ref[3]);

			if (strategy == 1 || strategy == 3)
			{
				t = (j % 16) / 2;
				if (ref[0] <= a)
					bytes[0][t] = 1;
				else
					bytes[0][t] = 0;

				if (ref[1] <= b)
					bytes[1][t] = 1;
				else
					bytes[1][t] = 0;

				if (ref[2] <= c)
					bytes[2][t] = 1;
				else
					bytes[2][t] = 0;

				if (ref[3] <= d)
					bytes[3][t] = 1;
				else
					bytes[3][t] = 0;

				//		if(i==182 && j==834)
				//		  printf("\n Encrypted bits-> bit0=%d, bit1=%d, bit2=%d, bit3=%d. ",bytes[0][t],bytes[1][t],bytes[2][t],bytes[3][t]);

				//CREATING DECRYPTED BIT SET
				bytes[0][t] = bytes[0][t] ^ bit[0];
				bytes[1][t] = bytes[1][t] ^ bit[1] ^ 1;
				bytes[2][t] = bytes[2][t] ^ bit[2];
				bytes[3][t] = bytes[3][t] ^ bit[3] ^ 1;
				//		if(i==182 && j==834)
				//		  printf("\n Decrypted bits-> bit0=%d, bit1=%d, bit2=%d, bit3=%d. ",bytes[0][t],bytes[1][t],bytes[2][t],bytes[3][t]);

				if (j % 16 == 14)
				{ /*	int q,p;
				if(lgo_row13==0 && lgo_col13<4)
				{	printf("\n Unpack. ");
					for(p=0;p<4;p++)
					{	printf("\n");
						for(q=0;q<8;q++)				
						    printf("%d ",bytes[p][q]);
					}
				}*/

					int i, t, k;
					for (i = 0; i < 4; i++)
					{
						k = val = 0;
						for (t = 7; t >= 0; t--)
						{
							val = val + bytes[i][t] * pow(2, k);
							k++;
						}
						fprintf(l[i], "%d %c", val, '\n');
					}

					lgo_col13++;
					if (lgo_col13 == l_dim[0].col * 3) //RESETTING COLUMN NO. WHEN IT REACHES THE END POINT
					{
						lgo_row13++;
						lgo_col13 = 0;
					}

					if (lgo_row13 == l_dim[0].row && lgo_col13 == 0)
					{
						fclose(l[0]);
						fclose(l[1]);
						fclose(l[2]);
						fclose(l[3]);
						//	printf("\n seg_cnt=%d. ",seg_cnt);
						return;
					} //BIT INSERTION FINISHED
				}
			}

			if (strategy == 2)
			{
				if (j % 4 == 0)
				{
					if (ref[0] <= a)
						bytes[0][0] = 1;
					else
						bytes[0][0] = 0;

					if (ref[1] <= b)
						bytes[0][1] = 1;
					else
						bytes[0][1] = 0;

					if (ref[2] <= c)
						bytes[0][2] = 1;
					else
						bytes[0][2] = 0;

					if (ref[3] <= d)
						bytes[0][3] = 1;
					else
						bytes[0][3] = 0;

					//		if(i==182 && j==834)
					//		  printf("\n Encrypted bits-> bit0=%d, bit1=%d, bit2=%d, bit3=%d. ",bytes[0][0],bytes[0][1],bytes[0][2],bytes[0][3]);

					bytes[0][0] = bytes[0][0] ^ bit[0];
					bytes[0][1] = bytes[0][1] ^ bit[1] ^ 1; //CREATING DECRYPTED BIT SET
					bytes[0][2] = bytes[0][2] ^ bit[2];
					bytes[0][3] = bytes[0][3] ^ bit[3] ^ 1;
					//		if(i==182 && j==834)
					//		  printf("\n Decrypted bits-> bit0=%d, bit1=%d, bit2=%d, bit3=%d. ",bytes[0][0],bytes[0][1],bytes[0][2],bytes[0][3]);
				}
				else
				{
					if (ref[0] <= a)
						bytes[0][4] = 1;
					else
						bytes[0][4] = 0;

					if (ref[1] <= b)
						bytes[0][5] = 1;
					else
						bytes[0][5] = 0;

					if (ref[2] <= c)
						bytes[0][6] = 1;
					else
						bytes[0][6] = 0;

					if (ref[3] <= d)
						bytes[0][7] = 1;
					else
						bytes[0][7] = 0;

					//		if(i==182 && j==834)
					//		  printf("\n Encrypted bits-> bit0=%d, bit1=%d, bit2=%d, bit3=%d. ",bytes[0][0],bytes[0][1],bytes[0][2],bytes[0][3]);

					bytes[0][4] = bytes[0][4] ^ bit[0];
					bytes[0][5] = bytes[0][5] ^ bit[1] ^ 1; //CREATING DECRYPTED BIT SET
					bytes[0][6] = bytes[0][6] ^ bit[2];
					bytes[0][7] = bytes[0][7] ^ bit[3] ^ 1;
					//		if(i==182 && j==834)
					//		  printf("\n Decrypted bits-> bit0=%d, bit1=%d, bit2=%d, bit3=%d. ",bytes[0][0],bytes[0][1],bytes[0][2],bytes[0][3]);

					int t, k;
					val = k = 0;
					for (t = 7; t >= 0; t--)
					{
						val = val + bytes[0][t] * pow(2, k);
						k++;
						if (j == 0 && i < 100 && n == 0)
							printf("%d.", bytes[0][t]);
					}
					if (j == 0 && i < 100 && n == 0)
						printf(",");
					fprintf(l[0], "%d %c", val, '\n');

					lgo_col2++;
					if (lgo_col2 == l_dim[n].col * 3)
					{
						lgo_col2 = 0;
						lgo_row2++;
					}
					if (lgo_row2 == l_dim[n].row)
					{
						n++;
						lgo_row2 = 0;
						fclose(l[0]);
						if (n < 4)
							l[0] = fopen(headers[n], "a+");
					}
					if (n == 4)
					{ //	printf("\n seg_cnt=%d. ",seg_cnt);
						return;
					} //BIT INSERTION FINISHED
				}
			}

			seg_cnt++;
		}
	}
}

int generate_bit(int bits[], int bytes[][9], int u, int j)
{
	static int lgo_row13 = 0, lgo_col13 = 0, lgo_row2 = 0, lgo_col2 = 0, n = 0;
	int i;

	// if(u==366 && j==160 && strategy==2)
	//   printf("\n val=%d. ",rgb_logo[0][lgo_row2][lgo_col2]);

	if (strategy == 1 || strategy == 3)
	{
		if (j % 16 == 0) //WHEN 1st BIT OF ANY LOGO TO BE INSERTED
		{
			i = 7;

			bytes[0][8] = rgb_logo[0][lgo_row13][lgo_col13];
			bytes[1][8] = rgb_logo[1][lgo_row13][lgo_col13];
			bytes[2][8] = rgb_logo[2][lgo_row13][lgo_col13];
			bytes[3][8] = rgb_logo[3][lgo_row13][lgo_col13];
			while (i >= 0) //ASCII TO BINARY CONVERSION OF LOGO ELEMENTS
			{
				bytes[0][i] = bytes[0][8] % 2;
				bytes[1][i] = bytes[1][8] % 2;
				bytes[2][i] = bytes[2][8] % 2;
				bytes[3][i] = bytes[3][8] % 2;
				bytes[0][8] /= 2;
				bytes[1][8] /= 2;
				bytes[2][8] /= 2;
				bytes[3][8] /= 2;
				i--;
			}
			bits[0] = bytes[0][0]; //CREATING BIT SET FOR INSERTION
			bits[1] = bytes[1][0];
			bits[2] = bytes[2][0];
			bits[3] = bytes[3][0];

			/*		if(lgo_row13==0 && lgo_col13<4)
		{	printf("\n Pack. ");
			for(i=0;i<4;i++)
			{	printf("\n");
				for(int u=0;u<8;u++)
			 	    printf("%d ",bytes[i][u]);
			}
		}*/

			lgo_col13++;
			if (lgo_col13 == l_dim[0].col * 3) //RESETTING COLUMN NO. WHEN IT REACHES THE END POINT
			{
				lgo_row13++;
				lgo_col13 = 0;
			}
		}
		else //WHEN 2nd TO 8th LOGO BITS ARE TO BE INSERTED
		{
			i = (j % 16) / 2;
			bits[0] = bytes[0][i];
			bits[1] = bytes[1][i];
			bits[2] = bytes[2][i];
			bits[3] = bytes[3][i];

			if (lgo_row13 == l_dim[0].row && lgo_col13 == 0 & i == 7)
				return 0; //BIT INSERTION FINISHED
		}
	}
	else if (strategy == 2)
	{
		if (j % 4 == 0)
		{
			int i = 7;

			bytes[0][8] = rgb_logo[n][lgo_row2][lgo_col2];
			while (i >= 0)
			{
				bytes[0][i] = bytes[0][8] % 2;
				bytes[0][8] /= 2;
				i--;
			}

			bits[0] = bytes[0][0];
			bits[1] = bytes[0][1];
			bits[2] = bytes[0][2];
			bits[3] = bytes[0][3];

			lgo_col2++;
			if (lgo_col2 == l_dim[0].col * 3)
			{
				lgo_row2++;
				lgo_col2 = 0;
			}
		}
		else
		{
			bits[0] = bytes[0][4];
			bits[1] = bytes[0][5];
			bits[2] = bytes[0][6];
			bits[3] = bytes[0][7];

			if (lgo_row2 == l_dim[0].row && lgo_col2 == 0)
			{
				n++;
				lgo_row2 = 0;
			}
			if (n == 4)
				return 0;
		}
	}
	return 1;
}

void create_stego()
{
	FILE *f;
	int i, j, nw_lne_cnt = 0, k, t, p;
	char c, buf[5];

	f = fopen(headers[LOGO], "a+");
	while (1)
	{
		if (nw_lne_cnt == 4)
			break;
		fscanf(f, "%c", &c);
		//	printf("%c, ",c);
		if (c == '\n')
			nw_lne_cnt++;
		;
	}

	for (i = 0; i < l_dim[LOGO].row; i++)
	{
		for (j = 0; j < l_dim[LOGO].col * 3; j++)
		{
			fprintf(f, "%d%c", rgb_image[i][j], '\n');
		}
	}
	fclose(f);
}

void check_tolerance(int u)
{
	int i, j;
	FILE *f;

	f = fopen(headers[LOGO], "a+");
	for (i = 0; i < l_dim[LOGO].row; i++)
	{
		for (j = 0; j < l_dim[LOGO].col * 3; j++)
		{
			rgb_image[i][j] += u;
			fprintf(f, "%d%c", rgb_image[i][j], '\n');
		}
	}
}
