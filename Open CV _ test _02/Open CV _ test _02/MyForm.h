#pragma once
#define _USE_MATH_DEFINES
#include "clusters.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <windows.h>
#include <math.h>
#include <vector>
//#include <>
//#include <opencv2\core\core.hpp>

//using namespace cv;

namespace Open_CV___test__02 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Runtime::InteropServices;
	//using namespace System::String;
	using namespace std;
	using namespace cv;


	struct jpeg_bloc
	{
		vector<int> v;
	};


	double cf(int f)
	{
		if (f == 0) return 1.0 / sqrt(2);
		if (f > 0) return 1;
		throw - 1;
	}



	Mat src;
	Mat srcOriginal;
	Cluster *clusters;
	int k;


	double *** Material_to_rgb_array(Mat m)
	{
		double *** ret_m;

		ret_m = new double**[3]; // R G B


		for (int j = 0; j < 3; j++)
		{
			ret_m[j] = new double *[m.rows];
			for (int i = 0; i < m.rows; i++)
			{
				ret_m[j][i] = new double[m.cols];
			}
		}


		for (int i = 0; i < m.rows; i++)
			for (int j = 0; j < m.cols; j++)
			{
				ret_m[0][i][j] = m.at<Vec3b>(i, j)[2];
				ret_m[1][i][j] = m.at<Vec3b>(i, j)[1];
				ret_m[2][i][j] = m.at<Vec3b>(i, j)[0];

			}	

		return ret_m;
	}



	double clamp(double min, double max, double value)
	{
		if (value > max) return max;
		if (value < min) return min;
		return value;	
	}



	double *** Material_to_YCbCr_array(Mat m)
	{
		double *** img = Material_to_rgb_array(m); // ��� �������

		double *** ret_m;
		ret_m = new double**[3]; // Y CB CR
		
		for (int j = 0; j < 3; j++)
		{
			ret_m[j] = new double *[m.rows];
			for (int i = 0; i < m.rows; i++)
			{
				ret_m[j][i] = new double[m.cols];
			}
		}
		
		for (int i = 0; i < m.rows; i++)
			for (int j = 0; j < m.cols; j++)
			{
				ret_m[0][i][j] = 0 + (0.299 * img[0][i][j]) + (0.587 * img[1][i][j]) + (0.114 * img[2][i][j]); // �������
				ret_m[1][i][j] = 128 - (0.168736 * img[0][i][j]) - (0.331264 * img[1][i][j]) + (0.5 * img[2][i][j]); //Col
				ret_m[2][i][j] = 128 + (0.5 * img[0][i][j]) - (0.418688 * img[1][i][j]) - (0.081312 * img[2][i][j]); // Col
			}
		
		return ret_m;
	}




	double *** YCbCr_to_array_rgb(double *** y, int w, int h)
	{
	
		double *** ret_m;
		ret_m = new double**[3]; // R G B

		for (int j = 0; j < 3; j++)
		{
			ret_m[j] = new double *[h];
			for (int i = 0; i < h; i++)
			{
				ret_m[j][i] = new double[w];
			}
		}


		for (int i = 0; i < h; i++)
			for (int j = 0; j < w; j++)
			{
				ret_m[0][i][j] = y[0][i][j] + 1.402 * (y[2][i][j] - 128);
				ret_m[1][i][j] = y[0][i][j] - 0.34414 * (y[1][i][j] - 128) - 0.71414 * (y[2][i][j] - 128);
				ret_m[2][i][j] = y[0][i][j] + 1.772 * (y[1][i][j] - 128);
				
				ret_m[0][i][j] = clamp(0, 255, ret_m[0][i][j]);
				ret_m[1][i][j] = clamp(0, 255, ret_m[1][i][j]);
				ret_m[2][i][j] = clamp(0, 255, ret_m[2][i][j]);


			}
	
		return ret_m;
	
	
	}


	void transpose(double ** matrix, int n) //���� int matrix[][5], ���� int (*matrix)[5]
	{
		double t;
		for (int i = 0; i < n; ++i)
		{
			for (int j = i; j < n; ++j)
			{
				t = matrix[i][j];
				matrix[i][j] = matrix[j][i];
				matrix[j][i] = t;
			}
		}
	}











	double** MultiplyMatrix(double** A, double** B, int size)
	{

		double **res = new double*[size];

		for (int i = 0; i < size; i++)
			res[i] = new double[size];


		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				res[i][j] = 0.0;
				for (int k = 0; k < size; k++)
				{
					res[i][j] += A[i][k] * B[k][j];

				}

				//round(res[i][j]);
			}
		}

		return res;
	}






	double **DCTm;
	double **DCTmT;


	//for i: = 0 to 8 do

		//for j : = 0 to 8 do

			//Q[i, j] = 1 + ((1 + i + j)*q);

	double ** genQx8(double _q = 2) // q = 2
	{
		double ** q;
		q = new double*[8];
		for (int i = 0; i < 8; i++)
		{
			q[i] = new double[8];
		}

		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++)
				q[i][j] = 1 + ((1 + i + j) * _q);	

		return q;
	}



	Mat rgb_array_to_material(double *** rmat, int h, int w)
	{
		Mat out = Mat(h, w, src.type());

		for (int i = 0; i < h; i++)
			for (int j = 0; j < w; j++)
			{
				out.at<Vec3b>(i, j)[2] = rmat[0][i][j];
				out.at<Vec3b>(i, j)[1] = rmat[1][i][j];
				out.at<Vec3b>(i, j)[0] = rmat[2][i][j];
			}
	
		return out;
	}






	double ** genDTCx8()
	{
		double **ret_m;
		ret_m = new double *[8];
		for (int i = 0; i < 8; i++)
		{
			ret_m[i] = new double[8];
		}




		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if (i == 0)
				{
					ret_m[i][j] = 1 / sqrt(8);
				}
				else
				{
					ret_m[i][j] = sqrt(2.0 / 8.0) * Math::Cos((2.0*(double)j+1.0)*(double)i*M_PI/(2.0*8.0));
				}
				
			}
		}

		return ret_m;
	}



	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//


		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}




		uchar get_max(int a, int b, int c) {
			return  max(a, max(b, c));
		}

		uchar get_min(int a, int b, int c) {
			return min(a, min(b, c));
		}





		Mat toGrayScale(Mat image)
		{
			uchar lightness;

			for (int i = 0; i < image.rows; ++i)
				for (int j = 0; j < image.cols; ++j) {
					Vec3b intensity = image.at<Vec3b>(i, j);
					uchar blue = intensity.val[0];
					uchar green = intensity.val[1];
					uchar red = intensity.val[2];

					lightness = (get_max(blue, green, red) + get_min(blue, green, red)) / 2;

					image.at<Vec3b>(i, j) = Vec3b(lightness, lightness, lightness);
				}


			return image;
		}



		double uniform(double sigma) {
			//sigma = 10;
			return sigma * sqrt(3) * ((2 * (rand() % 100) / 100) - 1);
		}


		Mat addNoize(Mat src, double sigma, int percent)
		{
			for (int i = 0; i < src.rows; ++i)
				for (int j = 0; j < src.cols; ++j) {
					if (rand() % 100 <= percent)
						src.at<Vec3b>(i, j) += Vec3b(uniform(sigma), uniform(sigma), uniform(sigma));
				}
			return src;
		}



		float Similarity(Mat img1, Mat img2)
		{

			float val1 = 0.0;

			for (int i = 0; i < img1.rows; ++i)
				for (int j = 0; j < img1.cols; ++j)
				{
					val1 += pow(img1.at<uchar>(i, j) - img2.at<uchar>(i, j), 2);
				}

			return 1.0 - sqrt(val1) / (img1.rows *img1.cols);
		}




		Mat mid_filtration(Mat src, int sigma)
		{


			int WinSize = sigma;
			Vec3b res_color;
			int aveR, aveG, aveB;
			double sumR, sumG, sumB;
			int col, row, n, m;

			Mat n_mat = src;

			for (col = 0; col < src.cols; col++)
				for (row = 0; row < src.rows; row++)
				{
					sumB = 0;
					sumG = 0;
					sumR = 0;
					int count = 0;
					for (m = col - ((WinSize - 1) / 2); m <= (col + (WinSize - 1) / 2); m++) {
						if (m < 0) {
							//m = 0;
							continue;
						}
						if (m > src.cols - 1) {
							//m = image.cols - 1;
							continue;
						}
						for (n = row - ((WinSize - 1) / 2); n <= row + ((WinSize - 1) / 2); n++)
						{
							if (n < 0) {
								//n = 0;
								continue;
							}
							if (n > src.rows - 1) {
								continue;
								//n = image.rows - 1;
							}
							count++;
							res_color = src.at<Vec3b>(m, n);
							Vec3b color = src.at<Vec3b>(m, n);
							uchar blue = color.val[0];
							uchar green = color.val[1];
							uchar red = color.val[2];
							sumR += red;
							sumG += green;
							sumB += blue;
						}
					}
					aveR = (int)(sumR / count);
					aveG = (int)(sumG / count);
					aveB = (int)(sumB / count);
					n_mat.at<Vec3b>(col, row) = Vec3b(aveB, aveG, aveR);
				}

			return n_mat;

		}




		Bitmap^ toBitMapPoint(Mat src)
		{
			// ------- Mat TO BitMap^ func
			Bitmap^ bit = gcnew Bitmap(src.cols, src.rows);

			for (int i = 0; i < src.cols; i++)
				for (int j = 0; j < src.rows; j++)
				{
					Vec3b color = src.at<Vec3b>(j, i);
					bit->SetPixel(i, j, Color().FromArgb(color.val[2], color.val[1], color.val[0]));
				}
			return bit;
		}







	private: System::Windows::Forms::PictureBox^  pictureBox1;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::PictureBox^  pictureBox2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::Button^  button4;
	private: System::Windows::Forms::Button^  button5;
	private: System::Windows::Forms::Button^  button6;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Button^  button7;
	private: System::Windows::Forms::Button^  button8;
private: System::Windows::Forms::PictureBox^  pictureBox3;
private: System::Windows::Forms::Label^  label3;
private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::Label^  label6;
private: System::Windows::Forms::Label^  label7;
private: System::Windows::Forms::Label^  label8;
private: System::Windows::Forms::NumericUpDown^  numericUpDown1;
private: System::Windows::Forms::NumericUpDown^  numericUpDown3;
private: System::Windows::Forms::NumericUpDown^  numericUpDown2;
private: System::Windows::Forms::NumericUpDown^  numericUpDown4;
private: System::Windows::Forms::NumericUpDown^  numericUpDown5;
private: System::Windows::Forms::PictureBox^  pictureBox4;
private: System::Windows::Forms::Button^  button9;
private: System::Windows::Forms::Button^  button10;
private: System::Windows::Forms::NumericUpDown^  numericUpDown6;
private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::Label^  label11;



	protected:







	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->pictureBox2 = (gcnew System::Windows::Forms::PictureBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->button5 = (gcnew System::Windows::Forms::Button());
			this->button6 = (gcnew System::Windows::Forms::Button());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->button7 = (gcnew System::Windows::Forms::Button());
			this->button8 = (gcnew System::Windows::Forms::Button());
			this->pictureBox3 = (gcnew System::Windows::Forms::PictureBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->numericUpDown1 = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDown3 = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDown2 = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDown4 = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDown5 = (gcnew System::Windows::Forms::NumericUpDown());
			this->pictureBox4 = (gcnew System::Windows::Forms::PictureBox());
			this->button9 = (gcnew System::Windows::Forms::Button());
			this->button10 = (gcnew System::Windows::Forms::Button());
			this->numericUpDown6 = (gcnew System::Windows::Forms::NumericUpDown());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->label11 = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox3))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown3))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown4))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown5))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox4))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown6))->BeginInit();
			this->SuspendLayout();
			// 
			// pictureBox1
			// 
			this->pictureBox1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBox1->Location = System::Drawing::Point(18, 18);
			this->pictureBox1->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(720, 753);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(18, 782);
			this->button1->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(112, 35);
			this->button1->TabIndex = 1;
			this->button1->Text = L"Browse...";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
			// 
			// pictureBox2
			// 
			this->pictureBox2->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBox2->Location = System::Drawing::Point(748, 18);
			this->pictureBox2->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->pictureBox2->Name = L"pictureBox2";
			this->pictureBox2->Size = System::Drawing::Size(320, 319);
			this->pictureBox2->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox2->TabIndex = 2;
			this->pictureBox2->TabStop = false;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(854, 343);
			this->label1->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(111, 20);
			this->label1->TabIndex = 3;
			this->label1->Text = L"Original Image";
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(808, 540);
			this->button2->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(188, 35);
			this->button2->TabIndex = 4;
			this->button2->Text = L"To Gray Scale";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &MyForm::button2_Click);
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(808, 629);
			this->button3->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(188, 35);
			this->button3->TabIndex = 5;
			this->button3->Text = L"Filtration";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &MyForm::button3_Click);
			// 
			// button4
			// 
			this->button4->Location = System::Drawing::Point(808, 454);
			this->button4->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(188, 35);
			this->button4->TabIndex = 6;
			this->button4->Text = L"Cmpr with original";
			this->button4->UseVisualStyleBackColor = true;
			this->button4->Click += gcnew System::EventHandler(this, &MyForm::button4_Click);
			// 
			// button5
			// 
			this->button5->Location = System::Drawing::Point(808, 398);
			this->button5->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->button5->Name = L"button5";
			this->button5->Size = System::Drawing::Size(188, 35);
			this->button5->TabIndex = 7;
			this->button5->Text = L"Make Original";
			this->button5->UseVisualStyleBackColor = true;
			this->button5->Click += gcnew System::EventHandler(this, &MyForm::button5_Click);
			// 
			// button6
			// 
			this->button6->Location = System::Drawing::Point(808, 585);
			this->button6->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->button6->Name = L"button6";
			this->button6->Size = System::Drawing::Size(188, 35);
			this->button6->TabIndex = 8;
			this->button6->Text = L"Add Noize";
			this->button6->UseVisualStyleBackColor = true;
			this->button6->Click += gcnew System::EventHandler(this, &MyForm::button6_Click);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(1436, 438);
			this->label2->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(94, 20);
			this->label2->TabIndex = 9;
			this->label2->Text = L"Similarity is: ";
			// 
			// button7
			// 
			this->button7->Location = System::Drawing::Point(18, 826);
			this->button7->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->button7->Name = L"button7";
			this->button7->Size = System::Drawing::Size(188, 35);
			this->button7->TabIndex = 10;
			this->button7->Text = L"Open in CV";
			this->button7->UseVisualStyleBackColor = true;
			this->button7->Click += gcnew System::EventHandler(this, &MyForm::button7_Click);
			// 
			// button8
			// 
			this->button8->Location = System::Drawing::Point(1144, 398);
			this->button8->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->button8->Name = L"button8";
			this->button8->Size = System::Drawing::Size(188, 35);
			this->button8->TabIndex = 11;
			this->button8->Text = L"Clust";
			this->button8->UseVisualStyleBackColor = true;
			this->button8->Click += gcnew System::EventHandler(this, &MyForm::button8_Click);
			// 
			// pictureBox3
			// 
			this->pictureBox3->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBox3->Location = System::Drawing::Point(1078, 18);
			this->pictureBox3->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->pictureBox3->Name = L"pictureBox3";
			this->pictureBox3->Size = System::Drawing::Size(320, 319);
			this->pictureBox3->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox3->TabIndex = 12;
			this->pictureBox3->TabStop = false;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(1186, 343);
			this->label3->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(126, 20);
			this->label3->TabIndex = 13;
			this->label3->Text = L"Clustered Image";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(1035, 483);
			this->label4->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(103, 20);
			this->label4->TabIndex = 14;
			this->label4->Text = L"Cluster count";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(1035, 540);
			this->label5->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(86, 20);
			this->label5->TabIndex = 15;
			this->label5->Text = L"Color coeff";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(1035, 585);
			this->label6->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(112, 20);
			this->label6->TabIndex = 16;
			this->label6->Text = L"Distance coeff";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(1035, 631);
			this->label7->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(101, 20);
			this->label7->TabIndex = 17;
			this->label7->Text = L"Dig color dec";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(1035, 671);
			this->label8->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(92, 20);
			this->label8->TabIndex = 18;
			this->label8->Text = L"Dig dist dec";
			// 
			// numericUpDown1
			// 
			this->numericUpDown1->Location = System::Drawing::Point(1152, 669);
			this->numericUpDown1->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->numericUpDown1->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 5, 0, 0, 0 });
			this->numericUpDown1->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->numericUpDown1->Name = L"numericUpDown1";
			this->numericUpDown1->Size = System::Drawing::Size(180, 26);
			this->numericUpDown1->TabIndex = 19;
			this->numericUpDown1->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			// 
			// numericUpDown3
			// 
			this->numericUpDown3->Location = System::Drawing::Point(1152, 629);
			this->numericUpDown3->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->numericUpDown3->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 5, 0, 0, 0 });
			this->numericUpDown3->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->numericUpDown3->Name = L"numericUpDown3";
			this->numericUpDown3->Size = System::Drawing::Size(180, 26);
			this->numericUpDown3->TabIndex = 21;
			this->numericUpDown3->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			// 
			// numericUpDown2
			// 
			this->numericUpDown2->Location = System::Drawing::Point(1152, 472);
			this->numericUpDown2->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->numericUpDown2->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
			this->numericUpDown2->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 2, 0, 0, 0 });
			this->numericUpDown2->Name = L"numericUpDown2";
			this->numericUpDown2->Size = System::Drawing::Size(180, 26);
			this->numericUpDown2->TabIndex = 22;
			this->numericUpDown2->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 5, 0, 0, 0 });
			this->numericUpDown2->ValueChanged += gcnew System::EventHandler(this, &MyForm::numericUpDown2_ValueChanged);
			// 
			// numericUpDown4
			// 
			this->numericUpDown4->Location = System::Drawing::Point(1152, 537);
			this->numericUpDown4->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->numericUpDown4->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->numericUpDown4->Name = L"numericUpDown4";
			this->numericUpDown4->Size = System::Drawing::Size(180, 26);
			this->numericUpDown4->TabIndex = 23;
			this->numericUpDown4->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			// 
			// numericUpDown5
			// 
			this->numericUpDown5->Location = System::Drawing::Point(1152, 577);
			this->numericUpDown5->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->numericUpDown5->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->numericUpDown5->Name = L"numericUpDown5";
			this->numericUpDown5->Size = System::Drawing::Size(180, 26);
			this->numericUpDown5->TabIndex = 24;
			this->numericUpDown5->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			// 
			// pictureBox4
			// 
			this->pictureBox4->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBox4->Location = System::Drawing::Point(1408, 18);
			this->pictureBox4->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->pictureBox4->Name = L"pictureBox4";
			this->pictureBox4->Size = System::Drawing::Size(320, 319);
			this->pictureBox4->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox4->TabIndex = 25;
			this->pictureBox4->TabStop = false;
			this->pictureBox4->Click += gcnew System::EventHandler(this, &MyForm::pictureBox4_Click);
			// 
			// button9
			// 
			this->button9->Location = System::Drawing::Point(1474, 398);
			this->button9->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->button9->Name = L"button9";
			this->button9->Size = System::Drawing::Size(188, 35);
			this->button9->TabIndex = 26;
			this->button9->Text = L"JPEG";
			this->button9->UseVisualStyleBackColor = true;
			this->button9->Click += gcnew System::EventHandler(this, &MyForm::button9_Click);
			// 
			// button10
			// 
			this->button10->Location = System::Drawing::Point(1540, 692);
			this->button10->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->button10->Name = L"button10";
			this->button10->Size = System::Drawing::Size(188, 35);
			this->button10->TabIndex = 27;
			this->button10->Text = L"Test";
			this->button10->UseVisualStyleBackColor = true;
			this->button10->Click += gcnew System::EventHandler(this, &MyForm::button10_Click);
			// 
			// numericUpDown6
			// 
			this->numericUpDown6->Location = System::Drawing::Point(1152, 705);
			this->numericUpDown6->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->numericUpDown6->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 20, 0, 0, 0 });
			this->numericUpDown6->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->numericUpDown6->Name = L"numericUpDown6";
			this->numericUpDown6->Size = System::Drawing::Size(180, 26);
			this->numericUpDown6->TabIndex = 28;
			this->numericUpDown6->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(1000, 707);
			this->label9->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(144, 20);
			this->label9->TabIndex = 29;
			this->label9->Text = L"Quantization factor";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(1536, 342);
			this->label10->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(41, 20);
			this->label10->TabIndex = 30;
			this->label10->Text = L"DCT";
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(1536, 438);
			this->label11->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(18, 20);
			this->label11->TabIndex = 31;
			this->label11->Text = L"0";
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(9, 20);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1846, 878);
			this->Controls->Add(this->label11);
			this->Controls->Add(this->label10);
			this->Controls->Add(this->label9);
			this->Controls->Add(this->numericUpDown6);
			this->Controls->Add(this->button10);
			this->Controls->Add(this->button9);
			this->Controls->Add(this->pictureBox4);
			this->Controls->Add(this->numericUpDown5);
			this->Controls->Add(this->numericUpDown4);
			this->Controls->Add(this->numericUpDown2);
			this->Controls->Add(this->numericUpDown3);
			this->Controls->Add(this->numericUpDown1);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->pictureBox3);
			this->Controls->Add(this->button8);
			this->Controls->Add(this->button7);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->button6);
			this->Controls->Add(this->button5);
			this->Controls->Add(this->button4);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->pictureBox2);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->pictureBox1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox3))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown3))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown4))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown5))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox4))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown6))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion



	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {

		//Start to load image into the picture box and set as src Mat image
		OpenFileDialog^ opDialog = gcnew OpenFileDialog();
		opDialog->Filter = "Image(*.bmp; *.jpg; *.png)|*.bmp;*.jpg;*.png|All files (*.*)|*.*||";
		if (opDialog->ShowDialog() == System::Windows::Forms::DialogResult::Cancel)
		{
			return;
		}
		Bitmap^ bmpSrc = gcnew Bitmap(opDialog->FileName);
		pictureBox1->Image = bmpSrc;
		pictureBox1->Refresh();
		pictureBox2->Image = bmpSrc;
		pictureBox2->Refresh();
		src = imread(ConvertString2Char(opDialog->FileName));
		srcOriginal = imread(ConvertString2Char(opDialog->FileName));
	}

	private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {


		if (!pictureBox1->Image) return;


		src = toGrayScale(src);

		pictureBox1->Image = toBitMapPoint(src);
		pictureBox1->Refresh();
		imshow("Display window", src);

	}



	private: char* ConvertString2Char(System::String^ str)
	{
		char* str2 = (char*)(void*)Marshal::StringToHGlobalAnsi(str);
		return str2;
	}


	private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {

		if (!pictureBox1->Image) return;



		src = mid_filtration(src, 3);

		pictureBox1->Image = toBitMapPoint(src);
		pictureBox1->Refresh();
		imshow("Display window", src);

	}
	private: System::Void button6_Click(System::Object^  sender, System::EventArgs^  e) {


		if (!pictureBox1->Image) return;

		src = addNoize(src, 10, 20);

		pictureBox1->Image = toBitMapPoint(src);
		pictureBox1->Refresh();
		imshow("Display window", src);

	}
	private: System::Void button4_Click(System::Object^  sender, System::EventArgs^  e) {

		if (!pictureBox1->Image) return;


		float sim = Similarity(src, srcOriginal);

		label2->Text = "Similarity is: " + Convert::ToString(sim * 100) + "%";

	}
	private: System::Void button5_Click(System::Object^  sender, System::EventArgs^  e) {


		if (!pictureBox1->Image) return;

		srcOriginal = src.clone();
		pictureBox2->Image = toBitMapPoint(src);
		pictureBox2->Refresh();


	}
	private: System::Void button7_Click(System::Object^  sender, System::EventArgs^  e) {

		if (!pictureBox1->Image) return;
		imshow("Display window", src);

	}
	private: System::Void button8_Click(System::Object^  sender, System::EventArgs^  e) {
	
		
		
		
		
		
		
		
		
		
		k = 2;


		k = System::Convert::ToInt32(numericUpDown2->Value);

		double dist_dig = System::Convert::ToInt32(numericUpDown1->Value);
		double col_dig = System::Convert::ToInt32(numericUpDown3->Value);;
		double col_coef = System::Convert::ToInt32(numericUpDown4->Value);;
		double dist_coef = System::Convert::ToInt32(numericUpDown5->Value);;


		double standart_dist_mul;



		//d = 0.. sqrt(n*n + m*m) * x
		//c = 0.. 255*sqrt(3)
		//255*sqrt(3) == sqrt(n*n + m*m) * x
		// => x ==    255*sqrt(3) / sqrt(n*n + m*m)
		
		double x;

		int n = src.cols;
		int m = src.rows;
		x = (255 * sqrt(3)) / sqrt(n * n + m * m);

		standart_dist_mul = x;

		double max = sqrt(n * n + m * m) * 255 * sqrt(3);
		dist_coef *= standart_dist_mul;

		clusters = new Cluster[k];
		vector<POINT> p;

		Mat Cluss_img;

		for (int i = 0; i < src.cols; i++)
			for (int j = 0; j < src.rows; j++)
			{
				POINT c;
				c.x = j; c.y = i;
				p.push_back(c);
			}


		for (int i = 0; i < k; i++)
			for (int j = 0; j < clusters[i].scores.size(); j++)
			{
				POINT x = clusters[i].scores[j];

				int y = 0;
				y = y + 10;
			}

		Cluss_img = Cluster().Start(k, clusters, p, src, col_dig, dist_dig, col_coef, dist_coef, max);

	

		pictureBox3->Image = toBitMapPoint(Cluss_img);
		pictureBox3->Refresh();
	}
	private: System::Void numericUpDown2_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
	}
private: System::Void button9_Click(System::Object^  sender, System::EventArgs^  e) {

	if (!pictureBox1->Image) return;

	int h, w;
	
	h = srcOriginal.rows / 8;
	w = srcOriginal.cols / 8;
	
	if (int(srcOriginal.rows) % 8 != 0)
	{
		h = h * 8 + 8;
	}
	else
	{
		h = h * 8;
	}

	if (int(srcOriginal.cols) % 8 != 0)
	{
		w = w * 8 + 8;
	}
	else
	{
		w = w * 8;
	}

	Mat jpg_m = Mat(h, w, srcOriginal.type());

	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			jpg_m.at<Vec3b>(i, j) = srcOriginal.at<Vec3b>((int)clamp(0, srcOriginal.rows - 1, i), (int)clamp(0, srcOriginal.cols - 1, j));
		}
	}

	double *** RGB_mat;

	RGB_mat = Material_to_rgb_array(jpg_m);

	double *** Y_cb_cr_mat = Material_to_YCbCr_array(jpg_m);

	for (int i = 0; i < jpg_m.rows; i++)
	{
		for (int j = 0; j < jpg_m.cols; j++)
		{
			Y_cb_cr_mat[0][i][j] -= 128;
		}
	}

	double ** block;
	double ** tmp;


	jpeg_bloc **jpg_blocks;

	jpg_blocks = new jpeg_bloc*[h / 8];


	for (int i = 0; i < h / 8; i++)
	{
		jpg_blocks[i] = new jpeg_bloc[w / 8];
	}



	DCTm = genDTCx8();
	DCTmT = genDTCx8();
	transpose(DCTmT, 8);

	int _q = int(numericUpDown6->Value);

	double ** quant = genQx8(_q);

	block = new double*[8];
	for (int i = 0; i < 8; i++)
	{
		block[i] = new double[8];
	}




	int similarity = 0;

	for (int i = 0; i < h / 8; i++)
	{
		for (int j = 0; j < w / 8; j++)
		{
			// ������� ���� �������� � �������� ��������������
			for (int k = 0; k < 8; k++)
				for (int m = 0; m < 8; m++)
					block[k][m] = Y_cb_cr_mat[0][i * 8 + k][j * 8 + m];

		
			double **tmp;

			tmp = MultiplyMatrix(block, DCTmT, 8);

			double **res;

			res = MultiplyMatrix(DCTm, tmp, 8);

			// �������������. �����������

			for (int k = 0; k < 8; k++)
				for (int m = 0; m < 8; m++)
					res[k][m] /= quant[k][m];


			vector<int> res_v;
			int counter = 0;



			for (int k = 0; k < 8; k++) // ��������� �� ���������
			{
				for (int m = 0; m <= k; m++)
				{
					res_v.push_back((int)res[k - m][m]);
					counter++;
				}
			}



			for (int k = 8; k < 16; k++)
			{
				for (int m = 0; m <= k; m++)
				{
					if ((k - m) >= 0 && (k - m) < 8 && m < 8 && m >= 0)
					{
						res_v.push_back((int)res[k - m][m]);
						counter++;
					}
				}
			}

			vector<int> save_v;

			for (int m = res_v.size() - 1; m >= 0; m--)
			{
				if (abs(res_v[m]) > 0)
				{
					

					for (int k = 0; k <= m; k++)
					{
						save_v.push_back(res_v[k]);
					}
					break;

				}
			}
			similarity += save_v.size();
			
			// ��������� �� �������, ������ ����� ���. � ��������
			jpg_blocks[i][j].v = save_v;
		}


	}


	for (int i = 0; i < h / 8; i++) 
	{
		for (int j = 0; j < w / 8; j++)
		{

			int counter = 0;

			double **res = genDTCx8(); // ��������� ������ ���� �������������. ������� �������������� �������� 


			for (int k = 0; k < 8; k++) // ��������� �� ���������
			{
				for (int m = 0; m <= k; m++)
				{
					if (counter < jpg_blocks[i][j].v.size())
					{
						res[k - m][m] = jpg_blocks[i][j].v[counter];
					}
					else
					{
						res[k - m][m] = 0;
					}
					counter++;
				}
			}



			for (int k = 8; k < 16; k++)
			{
				for (int m = 0; m <= k; m++)
				{
					if ((k - m) >= 0 && (k - m) < 8 && m < 8 && m >= 0)
					{
						if (counter < jpg_blocks[i][j].v.size())
						{
							res[k - m][m] = jpg_blocks[i][j].v[counter];
						}
						else
						{
							res[k - m][m] = 0;
						}
						counter++;
					}
				}
			}

			// ���������. ����������

			for (int k = 0; k < 8; k++)
				for (int m = 0; m < 8; m++)
					res[k][m] *= quant[k][m];






			double **tmp1;
			tmp1 = MultiplyMatrix(res, DCTm, 8);

			tmp = MultiplyMatrix(DCTmT, tmp1, 8);


			for (int i = 0; i < 8; i++)
				for (int j = 0; j < 8; j++)
					tmp[i][j] += 128;

			for (int m = 0; m < 8; m++)
				for (int k = 0; k < 8; k++)
				{
					Y_cb_cr_mat[0][i * 8+ m][j * 8 + k] = tmp[m][k];
				}



		}


	}

	double *** depack_rgb = YCbCr_to_array_rgb(Y_cb_cr_mat, jpg_m.cols, jpg_m.rows);


	Mat jpgovno;

	jpgovno = rgb_array_to_material(depack_rgb, h, w);


	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			src.at<Vec3b>(i, j) = jpgovno.at<Vec3b>(i,j);
		}
	}
	pictureBox4->Image = toBitMapPoint(src);
	pictureBox3->Refresh();
	imshow("Display window", src);

	label11->Text = (Convert::ToString(similarity));


}
private: System::Void button10_Click(System::Object^  sender, System::EventArgs^  e) {

	double ***rgb;// = Mat_to_matrix(src);

	double*** y_cb_cr = Material_to_YCbCr_array(src);

	rgb = YCbCr_to_array_rgb(y_cb_cr, src.cols, src.rows);

	Mat sh = rgb_array_to_material(rgb, src.rows, src.cols);

	imshow("Display window", sh);




}
private: System::Void pictureBox4_Click(System::Object^  sender, System::EventArgs^  e) {
	
}
};
}
