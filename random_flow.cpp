﻿#include "Random_flow.h"
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <QtCore/qrandom.h>
#include <random>
#include <QRandomGenerator>
#include <QDateTime>
#include <Eigen/Dense>
#include "fftw3.h"
#include <stdlib.h>
#include <time.h>
#include <complex.h>

Random_flow_lib::Random_flow_lib()
{
    ic = 15.0;
    tl = 10;
    st = 1;
    n = 11;
    xl = 10;
    sl = 1;
    m = 11;
}

Random_flow_lib::~Random_flow_lib()
{

}

void Random_flow_lib::step_length(double step_length)
{
    sl = step_length;
}

void Random_flow_lib::step_time(double step_time)
{
    st = step_time;
}

double Random_flow_lib::show_sl()
{
    return sl;
}

double Random_flow_lib::show_st()
{
    return st;
}

void Random_flow_lib::x_length(double length)
{
    xl = length;
}

void Random_flow_lib::t_length(double length)
{
    tl = length;
}

double Random_flow_lib::show_xl()
{
    return xl;
}

double Random_flow_lib::show_tl()
{
    return tl;
}

void Random_flow_lib::set_n_m()
{
    n = std::floor(tl/st) + 1;
    m = std::floor(xl/sl) + 1;
}

int Random_flow_lib::show_n()
{
    return n;
}

int Random_flow_lib::show_m()
{
    return m;
}

void Random_flow_lib::initial_condition(double initial_condition)
{
    ic = initial_condition;
}

double Random_flow_lib::show_ic()
{
    return ic;
}

Random_one_dimension_boussinesq::Random_one_dimension_boussinesq() : Random_flow_lib()
{
    we = 0;
    we_x = 0;
    Sy = 0.1;
    K = 10;
    a = -1; //设置-1为默认值方便后续程序判别
    a_as = -1;
    ha = 1;
    h_l[0] = 1.0;
    h_l[1] = 18.0;
    h_r[0] = 2.0;
    h_r[1] = 0;
    series_number = 1;
    angle = 0;

    use_white_noise_time = false;
    use_white_noise_length = false;
    seed = 0;
    rand = new QRandomGenerator(seed);
}

Random_one_dimension_boussinesq::~Random_one_dimension_boussinesq()
{
    delete rand;
}

void Random_one_dimension_boussinesq::a_()
{
    a = (show_ic() * K) / Sy;
}

void Random_one_dimension_boussinesq::reference_thickness(double ha_)
{
    ha = ha_;
}

void Random_one_dimension_boussinesq::pressure_diffusion_coefficient(double a_)
{
    a = a_;
}

void Random_one_dimension_boussinesq::source_sink_expectation(double we_)
{
    we = we_;
}

void Random_one_dimension_boussinesq::source_sink_expectation_x(double we_)
{
    we_x = we_;
}

void Random_one_dimension_boussinesq::hydraulic_conductivity(double K_)
{
    K = K_;
}

void Random_one_dimension_boussinesq::specific_yield(double Sy_)
{
    Sy = Sy_;
}

void Random_one_dimension_boussinesq::r_boundary(double h, bool Dirichlet, bool Neumann, bool Robin)
{
    if(Dirichlet){
        h_r[0] = 1;
        h_r[1] = h;
    }
    else if(Neumann){
        h_r[0] = 2;
        h_r[1] = h;
    }
    else if(Robin){
        h_r[0] = 3;
        h_r[1] = h;
    }
}

void Random_one_dimension_boussinesq::l_boundary(double h, bool Dirichlet, bool Neumann, bool Robin)
{
    if(Dirichlet){
        h_l[0] = 1;
        h_l[1] = h;
    }
    else if(Neumann){
        h_l[0] = 2;
        h_l[1] = h;
    }
    else if(Robin){
        h_l[0] = 3;
        h_l[1] = h;
    }
}

void Random_one_dimension_boussinesq::random_source_sink_term()
{
    // 创建一个随机数引擎
    std::random_device rd;
    std::mt19937 gen(rd()); // 使用Mersenne Twister引擎
    // 定义三个随机数分布器
    std::uniform_int_distribution<> dis(1, 51); // 生成1到50之间的随机整数
    std::uniform_real_distribution<> dis_1(0, we); // 生成0~we的随机小数
    std::uniform_int_distribution<> dis_(0, 1); // 生成0或1的随机整数
    // 随机振幅
    double amplitude = dis_1(gen);
    // 随机周期
    double cycle = 0.0;
    while(true){
        cycle = show_tl() / dis(gen); // 依据香农采样定理采样频率必须大于信号频率的两倍
        if (cycle >= 3 * show_st())  // 所以信号周期的随机生成必须大于采样周期的两倍，本程序取三倍
            break;
    }
    set_list_source_sink_term(dis_(gen), amplitude, cycle);
}

void Random_one_dimension_boussinesq::set_list_source_sink_term(double type_function, double amplitude, double cycle)
{
    std::vector<double> Array1 = {type_function, amplitude, cycle};
    list_source_sink_term.push_back(Array1);
}

void Random_one_dimension_boussinesq::del_last_list_source_sink_term() //  删除源汇项列表的最后一项
{
    std::vector<std::vector<double>> list_source_sink_term_new(list_source_sink_term.size() - 1, std::vector<double>(3));
    for (unsigned long long i = 0; i < list_source_sink_term.size() - 1; i++) {
        for (unsigned long long j = 0; j < list_source_sink_term[i].size(); j++) {
            list_source_sink_term_new[i][j] = list_source_sink_term[i][j];
        }
    }
    list_source_sink_term = list_source_sink_term_new;
}

void Random_one_dimension_boussinesq::clear_list_source_sink_term()
{
    std::vector<std::vector<double>> list_source_sink_term_clear;
    list_source_sink_term = list_source_sink_term_clear;
}

void Random_one_dimension_boussinesq::random_source_sink_term_x()
{
    // 创建一个随机数引擎
    std::random_device rd;
    std::mt19937 gen(rd()); // 使用Mersenne Twister引擎
    // 定义两个随机数分布器
    std::uniform_int_distribution<> dis(1, 51); // 生成1到50之间的随机整数
    std::uniform_real_distribution<> dis_1(0, we_x); // 生成0~we的随机小数
    std::uniform_int_distribution<> dis_(0, 1); // 生成0或1的随机整数
    // 随机振幅
    double amplitude = dis_1(gen);
    // 随机周期
    double cycle = 0.0;
    while(true){
        cycle = show_xl() / dis(gen); // 依据香农采样定理采样频率必须大于信号频率的两倍
        if (cycle >= 3 * show_sl())  // 所以信号周期的随机生成必须大于采样周期的两倍，本程序取三倍
            break;
    }
    set_list_source_sink_term_x(dis_(gen), amplitude, cycle);
}

void Random_one_dimension_boussinesq::set_list_source_sink_term_x(double type_function, double amplitude, double cycle)
{
    std::vector<double> Array1 = {type_function, amplitude, cycle};
    list_source_sink_term_x.push_back(Array1);
}

void Random_one_dimension_boussinesq::del_last_list_source_sink_term_x() //  删除源汇项列表的最后一项
{
    std::vector<std::vector<double>> list_source_sink_term_new(list_source_sink_term_x.size() - 1, std::vector<double>(3));
    for (unsigned long long i = 0; i < list_source_sink_term_x.size() - 1; i++) {
        for (unsigned long long j = 0; j < list_source_sink_term_x[i].size(); j++) {
            list_source_sink_term_new[i][j] = list_source_sink_term_x[i][j];
        }
    }
    list_source_sink_term_x = list_source_sink_term_new;
}

void Random_one_dimension_boussinesq::clear_list_source_sink_term_x()
{
    std::vector<std::vector<double>> list_source_sink_term_clear;
    list_source_sink_term_x = list_source_sink_term_clear;
}

void Random_one_dimension_boussinesq::show()
{
    for (unsigned long long i = 0; i < list_source_sink_term.size(); i++) {
        for (unsigned long long j = 0; j < list_source_sink_term[i].size(); j++) {
            std::cout << list_source_sink_term[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

double Random_one_dimension_boussinesq:: source_sink_term(double t)
{
    if(use_white_noise_time == true)
    {
        return numberical_value_w[t/show_st()];
    }
    else
    {
        double w_ = we;
        for (unsigned long long i = 0; i < list_source_sink_term.size(); i++) {
            if(list_source_sink_term[i][0] == 0){
                w_ += list_source_sink_term[i][1] * std::sin(2 * 3.1514926 * (1 / list_source_sink_term[i][2]) * t);
            }
            else if(list_source_sink_term[i][0] == 1){
                w_ += list_source_sink_term[i][1] * std::cos(2 * 3.1514926 * (1 / list_source_sink_term[i][2]) * t);
            }
        };
        return w_;
    }

}

double Random_one_dimension_boussinesq:: source_sink_term_x(double x)
{
    double w_ = we_x; // 遵循叠加原理"随空间变换的源汇项初始值为0"
    for (unsigned long long i = 0; i < list_source_sink_term_x.size(); i++) {
        if(list_source_sink_term_x[i][0] == 0){
            w_ += list_source_sink_term_x[i][1] * std::sin(2 * 3.1514926 * (1 / list_source_sink_term_x[i][2]) * x);
        }
        else if(list_source_sink_term_x[i][0] == 1){
            w_ += list_source_sink_term_x[i][1] * std::cos(2 * 3.1514926 * (1 / list_source_sink_term_x[i][2]) * x);
        }
    };
    return w_;
}

std::vector<std::vector<double>> Random_one_dimension_boussinesq::share_list_source_sink_term()
{
    return list_source_sink_term;
}

std::vector<std::vector<double>> Random_one_dimension_boussinesq::share_list_source_sink_term_x()
{
    return list_source_sink_term_x;
}

Eigen::MatrixXd Random_one_dimension_boussinesq::solve(int how_to_solve)
{
    // 如果未设定压力扩散系数
    if(a==-1){
        a = K / Sy;
    }
    set_n_m(); // 设置X,T轴差分点数
    create_plate(); // 设置底板高程数组
    Eigen::MatrixXd H_ALL(show_n(), show_m());//创建一个矩阵，用于存放各个差分位置的水头值
    H_ALL.setZero();
    for(int k = 0; k < show_n(); k++) //对行(时间轴)进行扫描
    {
        int iteration_times = 0;  // 迭代运算次数计数
        Eigen::MatrixXd H_previous_iteration(1, show_m()); // 前次迭代的水头厚度(h-z)
        Eigen::MatrixXd H_previous_iteration_1(1, show_m()); // 前次迭代的水头高h
        H_previous_iteration.setZero();
        Eigen::MatrixXd H(show_m(), 1);
        H.setZero();
        // 迭代运算开始
        while (true) {
            Eigen::MatrixXd H_a(show_m(), show_m()); // 每一时刻的系数矩阵
            H_a.setZero();
            int l_a = 0;
            Eigen::MatrixXd H_b(show_m(), 1); // 每一时刻的常数矩阵
            H_b.setZero();
            if (iteration_times == 0 && k != 0){
                H_previous_iteration.row(0) = H_ALL.row(k - 1);  // 前次迭代的当前时刻水头数值,此处未开始计算，使用上一时刻的水头值进行近似
            }
            for(int i  = 0; i < show_m(); i++)  // 对列(X轴)进行扫描
            {
                // 时间边界赋值(初始条件）
                if(k == 0){
                    H_a(l_a, l_a) = 1;
                    H_b(l_a, 0) = show_ic();
                }
                // 左边界赋值
                else if((i - 1) < 0 && h_l[0] == 1){
                    H_a(l_a, l_a) = 1;
                    H_b(l_a, 0) = h_l[1];
                }
                else if((i - 1) < 0 && h_l[0] == 2){
                    // 源汇项赋值
                    H_b(l_a, 0) = - source_sink_term(k * show_st()) / K - source_sink_term_x(i * show_sl()) / K - Sy / (K * show_st()) * H_ALL(k - 1, i) -
                                  2 * show_sl() * h_l[1] * (H_previous_iteration(0, i) + h_l[1] * 0.5 * show_sl()) / (show_sl() * show_sl());
                    // 给位置为(i, k)处的水头赋上系数值
                    H_a(l_a, l_a) = -(H_previous_iteration(0, i + 1) + H_previous_iteration(0, i)) / (2 * show_sl() * show_sl()) -
                                    (H_previous_iteration(0,i) + h_l[1] * 0.5 * show_sl()) / (show_sl() * show_sl()) - Sy / (K * show_st());
                    // 给位置为(i+1, k)处的水头赋上系数值
                    H_a(l_a, l_a + 1) = (H_previous_iteration(0, i + 1) + H_previous_iteration(0, i)) / (
                                            2 * show_sl() * show_sl()) + (H_previous_iteration(0, i) + h_l[1] * 0.5 * show_sl()) / (show_sl() * show_sl());
                }
                // 右边界赋值
                else if((i + 1) == show_m() && h_r[0] == 1){
                    H_a(l_a, l_a) = 1;
                    H_b(l_a, 0) = h_r[1];
                }
                else if((i + 1) == show_m() && h_r[0] == 2){
                    // 源汇项赋值
                    H_b(l_a, 0) = - source_sink_term(k * show_st()) / K - source_sink_term_x(i * show_sl()) / K - Sy / (K * show_st()) * H_ALL(k - 1, i) +
                                  2 * show_sl() * h_r[1] * (H_previous_iteration(0, i) + h_r[1] * 0.5 * show_sl()) / (show_sl() * show_sl());
                    // 给位置为(i, k)处的水头赋上系数值
                    H_a(l_a, l_a) = - (H_previous_iteration(0, i) + h_r[1] * 0.5 * show_sl()) / (show_sl() * show_sl()) -
                                    (H_previous_iteration(0, i) + H_previous_iteration(0, i - 1)) / (2 * show_sl() * show_sl()) - Sy / (K *show_st());
                    // 给位置为(i-1, k)处的水头赋上系数值
                    H_a(l_a, l_a - 1) = (H_previous_iteration(0, i) + H_previous_iteration(0, i - 1)) / (
                                            2 * show_sl() * show_sl()) + (H_previous_iteration(0, i) + h_r[1] * 0.5 * show_sl()) / (show_sl() * show_sl());
                }
                // 非边界部分赋值
                else{
                    // 源汇项赋值
                    H_b(l_a, 0) = - source_sink_term(k * show_st()) / K - source_sink_term_x(i * show_sl()) / K - Sy / (K * show_st()) * H_ALL(k - 1, i);
                        // 给位置为(i, k)处的水头赋上系数值
                    H_a(l_a, l_a) = -(H_previous_iteration(0 ,i + 1) + H_previous_iteration(0, i)) / (2 * show_sl() * show_sl()) -
                                    (H_previous_iteration(0, i) + H_previous_iteration(0, i - 1)) / (2 * show_sl() * show_sl()) - Sy / (K * show_st());
                    // 给位置为(i-1，k)处的水头赋上系数值
                    H_a(l_a, l_a - 1) = (H_previous_iteration(0, i) + H_previous_iteration(0, i - 1)) / (2 * show_sl() * show_sl());
                    // 给位置为(i+1, k)处的水头赋上系数值
                    H_a(l_a, l_a + 1) = (H_previous_iteration(0, i + 1) + H_previous_iteration(0, i)) / (2 * show_sl() * show_sl());
                }
                l_a += 1;
            }

            if(how_to_solve == 1){ // 使用LU分解法进行求解
                H = H_a.lu().solve(H_b);
            }
            else if (how_to_solve == 2){ // 使用QR分解法进行求解
                H = H_a.householderQr().solve(H_b);
            }
            else if (how_to_solve == 3){ //使用追赶法进行求解
                H = solve_zhuiganfa(H_a, H_b);
            }
            if(k == 0){  // 第零时刻不参与迭代计算
                break;
            }
            // 判断是否满足精度需求
            int precision = 0;
            for(int u = 0; u < show_m(); u++){
                if (abs(H_previous_iteration_1(0, u) - H(u, 0)) > 0.001){
                    precision = 1;
                }
            }
            if (precision != 1){
                break;
            }
            else{
                iteration_times += 1;
                H_previous_iteration_1.row(0) = H.col(0);
                for(int o = 0;o < show_m(); o++){
                    H_previous_iteration.row(0)(o) = H.col(0)(o) - plate_elevation(o);
                }
                }

            if(iteration_times > 100)
            break;
        }
        //std::cout<<iteration_times<<std::endl;
        for(int o = 0; o < show_m(); o++)  // 对空间进行扫描，整合成所有适合的计算水头
            H_ALL(k, o) = H(o, 0);

    }
    return H_ALL;
}

Eigen::VectorXd Random_one_dimension_boussinesq::fast_fourier_transfrom(Eigen::MatrixXd solution, int n)
{
    set_n_m();
    // 创建输出数组，用于存储FFT结果
    fftw_complex *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * n);
    // 创建FFT计划
    fftw_plan plan = fftw_plan_dft_r2c_1d(n, solution.data(), out, FFTW_ESTIMATE);
    // 执行FFT计算
    fftw_execute(plan);

    // 归一化和取一半处理
    int N = std::round(n / 2);
    Eigen::VectorXd Amplitude(N); // 用于存储频谱的振幅
    for (int i = 0; i < N; i++) {
        Amplitude(i) = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]); // 取复数的绝对值得到振幅
        if(i == 0){
            Amplitude(i) =  Amplitude(i) / n; // 转换为真实振幅0;
        }
        else{
            Amplitude(i) = Amplitude(i) / (n / 2); // 转换为真实振幅
        }
    }
    // 销毁计划
    fftw_destroy_plan(plan);
    fftw_free(out);
    return Amplitude;
}

Eigen::VectorXd Random_one_dimension_boussinesq::power_spectral_density(Eigen::MatrixXd solution, int n)
{
    set_n_m();
    // 创建输出数组，用于存储FFT结果
    fftw_complex *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * n);
    // 创建FFT计划
    fftw_plan plan = fftw_plan_dft_r2c_1d(n, solution.data(), out, FFTW_ESTIMATE);
    // 执行FFT计算
    fftw_execute(plan);

    // 归一化和取一半处理
    int N = std::round(n / 2);
    Eigen::VectorXd Amplitude(N); // 用于功率谱密度的振幅
    for (int i = 0; i < N; i++) {
        Amplitude(i) = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]); // 取复数的绝对值得到振幅
        if(i == 0){
            Amplitude(i) =  (Amplitude(i) / n) * (Amplitude(i) / n) / show_tl(); // 转换为真实振幅0;Sx[k]=(1/N)∣X[k]∣^2
        }
        else{
            Amplitude(i) = (Amplitude(i) / (n / 2)) * (Amplitude(i) / (n / 2)) / show_tl(); // 转换为真实振幅的模的平方
        }
    }
    // 销毁计划
    fftw_destroy_plan(plan);
    fftw_free(out);
    return Amplitude;
}

Eigen::VectorXd Random_one_dimension_boussinesq::amplitude_complete_fdm(Eigen::MatrixXd solution, int l) // l代表位置
{
    Eigen::VectorXd Amplitude_h_fft = fast_fourier_transfrom(solution.col(l), show_n());// flow.fast_fourier_transfrom(solve_fdm.col(a), flow.show_n())
    Eigen::VectorXd Amplitude_w(show_n());
    double t = 0.0;
    // 离散化源汇项赋值
    for(int i = 0; i < show_n(); i++){
        Amplitude_w[i] = source_sink_term(t);
        t += show_st();
    }
    Eigen::VectorXd Amplitude_w_fft = fast_fourier_transfrom(Amplitude_w, show_n());
    Eigen::VectorXd Amplitude_fdm(show_n() / 2);
    //std::cout<<"sbwycwds"<<std::endl;
    for(int j = 0; j < (show_n() / 2); j++){
        Amplitude_fdm[j] = fabs(Amplitude_h_fft[j] / Amplitude_w_fft[j]);
    }
    return Amplitude_fdm;
}

Eigen::VectorXd Random_one_dimension_boussinesq::amplitude_complete_analyze()
{
    a_();
    Eigen::VectorXd Amplitude_analyze(show_n() / 2);
    double e = 0.0;
    for(int i = 0; i < (show_n() / 2); i++){
        e = A(i * (1 / show_tl()));
        Amplitude_analyze[i] = e;
    }
    Amplitude_analyze[0] = Amplitude_analyze[1]; // 把不存在的0的位置给替换掉
    return Amplitude_analyze;
}

Eigen::MatrixXd Random_one_dimension_boussinesq::solve_zhuiganfa(Eigen::MatrixXd a,Eigen::MatrixXd b)
{
    Eigen::MatrixXd x(show_m(), 1); // 解值矩阵
    double *BB,*GG;
    int i;
    //为系数数组分配内存空间
    BB=new double[show_m()];
    GG=new double[show_m()];
    //系数追踪
    BB[0]=a(0,0);
    GG[0]=b(0)/BB[0];
    for(i=1;i<show_m();i++)
    {
        BB[i]=a(i,i)-a(i, i-1)*a(i-1, i)/BB[i-1];
        GG[i]=(b(i)-a(i, i-1)*GG[i-1])/BB[i];
    }
    //求解未知数
    for(i=(show_m()-1);i>=0;i--)
    {
        if(i==(show_m()-1))x(i)=GG[i];
        else  x(i)=GG[i]-a(i, i+1)*x(i+1)/BB[i];
    }
    delete[] BB;
    delete[] GG;
    return x;
}

std::complex<double> Random_one_dimension_boussinesq::M(double x, double w, double l)
{
    std::complex<double>r1(0, w/a);
    std::complex<double>r2(0, w/a);
    r1 = sqrt(r1);
    r2 = conj(r1); // 共轭复数
    std::complex<double> m;
    m = (r1 *exp(r1 * l)* exp(r2 * x) - r2 *exp(r2 * l)* exp(r1 * x)) / (r1 *exp(r1 * l) -  r2 *exp(r2 * l));
    return m;
}

double Random_one_dimension_boussinesq::A(double w){
    std::complex<double> z(0.0, w * show_xl() * show_xl() / a);
    std::complex<double> sqrt_z = std::sqrt(z);
    // 计算双曲余弦的反函数
    std::complex<double> acosh_z = std::acosh(sqrt_z);
    double result = (show_xl() * show_xl() / (a * Sy)) * abs((a / (w * show_xl() * show_xl())) *acosh_z);
    //std::complex<double>a_com(0, 1/(w * Sy));
    //std::complex<double>m = M(x, w, l);
    //std::complex<double>m_(m.real() - 1, m.imag());
    //cout<<m_<<endl;
    //double Amp = fabs(a_com * (m_));
    //return Amp;
    return result;

}

void Random_one_dimension_boussinesq::set_angle(double a)
{
    angle = a;
}

void Random_one_dimension_boussinesq::create_plate()
{
    plate.clear();
    double m = show_m();
    int sl = show_sl();
    for(int i = 0;i < m;i++)
    {
        plate.emplace_back(tan(angle * 3.1415926 / 180) * i * sl);
    }
}

double Random_one_dimension_boussinesq::plate_elevation(double i)
{
    return plate[i];
}

void Random_one_dimension_boussinesq::set_white_noise_time(int s) // 设置是否启用白噪声模式
{
    if(s==0) use_white_noise_time = false;
    else
    {
        set_n_m();
        numberical_value_w.clear();
        // 创建一个随机数引擎
        double t0 = time(0); // 使用当前系统时间戳为随机数种子
        std::mt19937 gen(t0); // 使用Mersenne Twister引擎
        // 定义两个随机数分布器
        std::uniform_real_distribution<> dis(0, 2 * we); // 生成-1 * we ~ 2 * we均匀分布的随机小数
        use_white_noise_time = true;
        for(int i=0;i<show_n();i++)
        {
            numberical_value_w.emplace_back(dis(gen));// 白噪声数值填充
        }
    }
}

double Random_one_dimension_boussinesq::actual_expectations_white_noise_time()
{
    double expectations = 0;
    for(double e : numberical_value_w)
    {
        expectations += e;
    }
    return expectations / show_n();
}
