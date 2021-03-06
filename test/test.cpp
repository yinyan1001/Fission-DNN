/*************************************************************************
	> File Name: test.cpp
	> Author: TDX 
	> Mail: sa614149@mail.ustc.edu.cn
	> Created Time: 2016年12月19日 星期一 22时55分04秒
 ************************************************************************/
#include"test.hpp"
#include<iostream>
#include"common/common.hpp"
#include"common/nDMatrix.hpp"

using namespace std;

template<>
void printf_NDMatrix_data(NDMatrix<float>*matrix)
{
    const int num = matrix->ND_num();
    const int channels = matrix->ND_channels();
    const int height = matrix->ND_height();
    const int width = matrix->ND_width();

    for(int n = 0; n < num; n++)
    {
        for(int c = 0; c < channels; c++)
        {
            for(int h = 0; h < height; h++)
            {
                for(int  w = 0; w < width; w++)
                {
                    cout<< matrix->data_at(n, c, h, w)<<" ";
                }

                cout<<endl;
            }
            cout<<endl;
        }
        cout<< endl;
        cout<<matrix->ND_shape_string()<<endl;
        //if(n == 1)
        //for(;;);
    }

    for(;;);
}

template<>
void printf_NDMatrix_data(NDMatrix<double>*matrix)
{
    const int num = matrix->ND_num();
    const int channels = matrix->ND_channels();
    const int height = matrix->ND_height();
    const int width = matrix->ND_width();

    for(int n = 0; n < num; n++)
    {
        for(int c = 0; c < channels; c++)
        {
            for(int h = 0; h < height; h++)
            {
                for(int  w = 0; w < width; w++)
                {
                    cout<< matrix->data_at(n, c, h, w)<<" ";
                }
                cout<<endl;
            }
        }

        cout<< endl;
        for(;;);
    }
}

template<>
void printf_NDMatrix_data(NDMatrix<int>*matrix)
{
    const int num = matrix->ND_num();
    const int channels = matrix->ND_channels();
    const int height = matrix->ND_height();
    const int width = matrix->ND_width();
    
    for(int n = 0; n < num; n++)
    {
        for(int c = 0; c < channels; c++)
        {
            for(int h = 0; h < height; h++)
            {
                for(int  w = 0; w < width; w++)
                {
                    cout<<n << " = "<<matrix->data_at(n, c, h, w)<<" ";
                }
            }
        }
    }
    cout<< endl;
}

//printf devData
void printf_devData(int number, int channels, int height, int width, float*A)
{
    float*tmpA;
    tmpA = (float*)malloc(number* channels*height* width* sizeof(float));
    CUDA_CHECK(cudaMemcpy(tmpA, A, number*channels*height*width*sizeof(float), cudaMemcpyDeviceToHost));

    for(int n = 0; n < number; n++)
    {
        for(int c= 0; c < channels; c++)
        {
            for(int h = 0; h < height; h++)
            {
                for(int w = 0; w < width; w++)
                {
                    cout<<tmpA[h*width + w + height*width* c + height* width * channels * n]<<" ";
                }
                cout<<endl;
            }
            cout<<endl;
        }       
    }

    cout<<number<<" "<<channels<<" "<<height<<" "<<width<<endl;
    for(;;);
}

template<>
void printf_NDMatrix_diff(NDMatrix<float>*matrix)
{
    const int num = matrix->ND_num();
    const int channels = matrix->ND_channels();
    const int height = matrix->ND_height();
    const int width = matrix->ND_width();

    for(int n = 0; n < num; n++)
    {
        for(int c = 0; c < channels; c++)
        {
            for(int h = 0; h < height; h++)
            {
                for(int  w = 0; w < width; w++)
                {
                    cout<< matrix->diff_at(n, c, h, w)<<" ";
                }

                cout<<endl;
            }
            cout<<endl;
        }
        cout<< endl;
        cout<<matrix->ND_shape_string()<<endl;
        //if(n == 1)
        //for(;;);
    }

    for(;;);
}
template<>
void printf_NDMatrix_diff(NDMatrix<double>*matrix)
{
    const int num = matrix->ND_num();
    const int channels = matrix->ND_channels();
    const int height = matrix->ND_height();
    const int width = matrix->ND_width();

    for(int n = 0; n < num; n++)
    {
        for(int c = 0; c < channels; c++)
        {
            for(int h = 0; h < height; h++)
            {
                for(int  w = 0; w < width; w++)
                {
                    cout<< matrix->diff_at(n, c, h, w)<<" ";
                }

                cout<<endl;
            }
            cout<<endl;
        }
        cout<< endl;
        cout<<matrix->ND_shape_string()<<endl;
        //if(n == 1)
        //for(;;);
    }

    for(;;);
}
template<>
void printf_NDMatrix_diff(NDMatrix<int>*matrix)
{
    const int num = matrix->ND_num();
    const int channels = matrix->ND_channels();
    const int height = matrix->ND_height();
    const int width = matrix->ND_width();

    for(int n = 0; n < num; n++)
    {
        for(int c = 0; c < channels; c++)
        {
            for(int h = 0; h < height; h++)
            {
                for(int  w = 0; w < width; w++)
                {
                    cout<< matrix->diff_at(n, c, h, w)<<" ";
                }

                cout<<endl;
            }
            cout<<endl;
        }
        cout<< endl;
        cout<<matrix->ND_shape_string()<<endl;
        //if(n == 1)
        //for(;;);
    }

    for(;;);
}
