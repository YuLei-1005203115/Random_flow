﻿#ifndef RANDOM_FLOW_H
#define RANDOM_FLOW_H

#include<string>
#include<vector>
#include<cmath>
#include<Eigen/Dense>
#include "fftw3.h"

class Random_flow_lib
{
private:
    std::string name_chinese = "潜水含水层随机流";
    double ic;  // 初始条件
    double tl; // 时间轴轴长 原则上单位为天
    double st; // 时间差分步长
    int n; // 时间轴差分点数目
    double xl; // X轴轴长 原则上单位为米
    double sl; // X轴差分步长
    int m; // X轴差分点数目

    double B=1; // 默认一维流的宽度为1

public:
    Random_flow_lib();
    ~Random_flow_lib();
    void step_length(double step_length);
    void step_time(double step_time);
    double show_sl();
    double show_st();
    void x_length(double length);
    void t_length(double length);
    double show_tl();
    double show_xl();
    void set_n_m();
    int show_n();
    int show_m();
    void initial_condition(double initial_condition);
    double show_ic();
    void width(double width);
};

class Random_one_dimension_boussinesq : public Random_flow_lib
{
private:
    double we; // 源汇项期望
    double Sy; // 给水度
    double K; // 渗透系数
    double a; // 压力扩散系数
    double a_as; // 解析解压力扩散系数
    double ha; // 参考厚度
    double h_r[2]; // 左右边界
    double h_l[2];
    int series_number; // 表示随机源汇项的傅里叶级数的项数
    std::vector<std::vector<double>> list_source_sink_term; // 制定一个不定长二维数组来实现对源汇项傅里叶级数的储存
    std::string name_chinese = "潜水含水层随机非稳定一维流";

public:
    Random_one_dimension_boussinesq();
    ~Random_one_dimension_boussinesq();
    void reference_thickness(double ha_);
    void pressure_diffusion_coefficient(double a_);
    void source_sink_expectation(double we_);
    void hydraulic_conductivity(double K_);
    void specific_yield(double Sy_);
    void r_boundary(double h, bool Dirichlet=false, bool Neumann=false, bool Robin=false);
    void l_boundary(double h, bool Dirichlet=false, bool Neumann=false, bool Robin=false);
    void random_source_sink_term();
    void set_list_source_sink_term(double type_function, double amplitude, double cycle);
    void del_last_list_source_sink_term();
    void clear_list_source_sink_term();
    void show();
    double source_sink_term(double x);
    std::vector<std::vector<double>> share_list_source_sink_term();
    Eigen::MatrixXd solve(int how_to_solve);
    Eigen::VectorXd fast_fourier_transfrom(Eigen::MatrixXd solution, int n);



};
#endif // RANDOM_FLOW_H
