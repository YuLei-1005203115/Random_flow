#include "randow_flow_window.h"
#include "ui_randow_flow_window.h"
#include "set_fdm.h"
#include "set_hydrogeological_parameter.h"
#include "set_new_wave.h"
#include "Random_flow.h"
#include <iostream>
#include <chrono>


Randow_flow_Window::Randow_flow_Window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Randow_flow_Window)
{
    ui->setupUi(this);
    step_length = 1;
    step_time = 1;
    how_to_solve = 1;
    hydraulic_conductivity = 10;
    specific_yield = 0.1;
    chart_head = new QChart();
    series_head = new QLineSeries();
    axis_head = new QValueAxis();
    axis_x = new QValueAxis();
    create_chart_head();
    chart_W = new QChart();
    series_W = new QLineSeries();
    axis_W = new QValueAxis();
    axis_w = new QValueAxis();
    create_chart_W();
    // 记录当前系统时间
    auto now = std::chrono::system_clock::now();
    // 输出当前时间
    std::time_t time_now = std::chrono::system_clock::to_time_t(now);
    QString str = std::ctime(&time_now);
    ui->textBrowser->append("当前程程序时间：" + str);

}

Randow_flow_Window::~Randow_flow_Window()
{
    delete ui;
    delete chart_head;
    delete series_head;
    delete axis_head;
    delete axis_x;
    delete chart_W;
    delete series_W;
    delete axis_W;
    delete axis_w;
    std::cout<<"game is over"<<std::endl;
}

void Randow_flow_Window::create_chart_head()
{
    ui->graphicsView->setChart(chart_head);
    chart_head->setTitle(tr("水头绘制曲线:示例"));
    axis_x->setRange(0, 10);
    axis_x->setLabelFormat("%.1f"); // 标签格式
    axis_x->setTickCount(11);
    axis_x->setMinorTickCount(2);
    axis_x->setTitleText("X轴(m)");

    axis_head->setLabelFormat("%.1f"); // 标签格式
    axis_head->setTickCount(11);
    axis_head->setMinorTickCount(2);
    axis_head->setTitleText("水头(m)");

    series_head->setName("水头曲线: 本图表为示例");
    series_head->append(0, 1);
    series_head->append(2, 7);
    series_head->append(6, 2);
    series_head->append(8, 9);

    chart_head->addSeries(series_head);
    chart_head->addAxis(axis_x, Qt::AlignBottom);
    chart_head->addAxis(axis_head, Qt::AlignLeft);
    series_head->attachAxis(axis_x);
    series_head->attachAxis(axis_head);

}

void Randow_flow_Window::create_chart_W()
{
    ui->graphicsView_W->setChart(chart_W);
    chart_W->setTitle(tr("源汇项绘制曲线:示例"));
    QMargins W;
    W.setBottom(0);
    W.setTop(0);
    W.setLeft(0);
    W.setRight(0);
    chart_W->setMargins(W);
    axis_w->setRange(0, 10);
    axis_w->setLabelFormat("%.1f"); // 标签格式
    axis_w->setTickCount(6);
    axis_w->setMinorTickCount(1);
    axis_w->setTitleText("T轴(d)");

    axis_W->setLabelFormat("%.1f"); // 标签格式
    axis_W->setTickCount(6);
    axis_W->setMinorTickCount(1);
    axis_W->setTitleText("水头(m)");

    for(int i = 0; i < 11; i++){
        series_W->append(i, std::sin(i));
    }

    chart_W->addSeries(series_W);
    chart_W->addAxis(axis_w, Qt::AlignBottom);
    chart_W->addAxis(axis_W, Qt::AlignLeft);
    series_W->attachAxis(axis_w);
    series_W->attachAxis(axis_W);
}
void Randow_flow_Window::left_boundary()
{
    QString str = QString::number(ui->doubleSpinBox_left_boundary->value());

    if(ui->comboBox_left_boundary->currentText() == "一类边界（给定水头）"){
        flow.l_boundary(ui->doubleSpinBox_left_boundary->value(), true, false, false);
        ui->textBrowser->append("左边界（给定水头边界）：\n水头值为：" + str);
    }
    else if(ui->comboBox_left_boundary->currentText() == "二类边界（给定通量）"){
        flow.l_boundary(ui->doubleSpinBox_left_boundary->value(), false, true, false);
        ui->textBrowser->append("左边界（给定通量边界）：\n通量值为：" + str);
    }
}

void Randow_flow_Window::right_boundary()
{
    QString str = QString::number(ui->doubleSpinBox_right_boundary->value());

    if(ui->comboBox_right_boundary->currentText() == "一类边界（给定水头）"){
        flow.r_boundary(ui->doubleSpinBox_right_boundary->value(), true, false, false);
        ui->textBrowser->append("右边界（给定水头边界）：\n水头值为：" + str);
    }
    else if(ui->comboBox_right_boundary->currentText() == "二类边界（给定通量）"){
        flow.r_boundary(ui->doubleSpinBox_right_boundary->value(), false, true, false);
        ui->textBrowser->append("右边界（给定通量边界）：\n通量值为：" + str);
    }
}

void Randow_flow_Window::set_time_choose_box() // 设置时刻选择条
{
    QString str_tl = QString::number(flow.show_tl());
    QString str_st = QString::number(flow.show_st());
    QString str_n = QString::number(flow.show_n());
    ui->spinBox_time->setMaximum(flow.show_n() - 1);
    ui->textBrowser_time->setPlainText("计算时长为：" + str_tl + "天，时间分割步长为：" + str_st + "天，共计有时刻" + str_n + "个。");
}

void Randow_flow_Window::set_location_choose_box()  // 设置位置选择条
{
    QString str_xl = QString::number(flow.show_xl());
    QString str_sl = QString::number(flow.show_sl());
    QString str_m = QString::number(flow.show_m());
    ui->spinBox_X->setMaximum(flow.show_m() - 1);
    ui->textBrowser_X->setPlainText("计算轴长为：" + str_xl + "米，空间分割步长为：" + str_sl + "米，共计有位置离散点" + str_m + "个。");
}

void Randow_flow_Window::solve()
{
    flow.initial_condition(ui->doubleSpinBox_ic->value());
    left_boundary();
    right_boundary();
    flow.x_length(ui->spinBox_x_length->value());
    flow.t_length(ui->spinBox_t_length->value());
    solve_fdm =  flow.solve(how_to_solve);
    ui->textBrowser->append("计算完毕");
    set_time_choose_box();
    set_location_choose_box();
}

void Randow_flow_Window::get_wave_info(double cycle, double amplitue) // 主窗口获得波动信息的槽函数
{
    flow.set_list_source_sink_term(0, amplitue, cycle);
    std::vector<std::vector<double>> list_source_sink_term = flow.share_list_source_sink_term();
    QString str = QString::number(ui->doubleSpinBox_rain->value()/ (1000 * 365));
    for (unsigned long long i = 0; i < list_source_sink_term.size(); i++) {
        if(list_source_sink_term[i][0] == 0){
            str += " + " + QString::number(list_source_sink_term[i][1]) + "sin((2pi / " + QString::number(list_source_sink_term[i][2]) + ") * t)";
        }
        else if(list_source_sink_term[i][0] == 1){
            str += " + " + QString::number(list_source_sink_term[i][1]) + "cos((2pi / " + QString::number(list_source_sink_term[i][2]) + ") * t)";
        }
    };
    ui->textBrowser_rain_function->clear();
    ui->textBrowser_rain_function->append(str);
}

void Randow_flow_Window::on_actionSet_fdm_triggered() // 此函数用于打开设置数值解求解设置的子窗口并设置数值
{
    Set_FDM *Set_FDM_window = new Set_FDM(this);
    Set_FDM_window->set_window(step_length, step_time, how_to_solve);
    int ret = Set_FDM_window->exec();
    if(ret == QDialog::Accepted){
        step_length = Set_FDM_window->step_length(); // 获取差分步长
        step_time =  Set_FDM_window->step_time(); // 获取差分时长
        Set_FDM_window->how_to_solve(); // 判断求解方式
        how_to_solve = Set_FDM_window->solve(); // 获取求解方式
        //std::cout<<step_length<<std::endl<<step_time<<std::endl<<how_to_solve<<std::endl;
        flow.step_length(step_length);
        flow.step_time(step_time);
    }
    delete Set_FDM_window;
}

void Randow_flow_Window::on_actionSet_hydrogeological_parameter_triggered() // 此函数用于打开设置水文地质学参数的子窗口并设置数值
{
    Set_hydrogeological_parameter *Set_hydrogeological_parameter_window = new Set_hydrogeological_parameter(this);
    Set_hydrogeological_parameter_window->set_window(hydraulic_conductivity, specific_yield);
    int ret = Set_hydrogeological_parameter_window->exec();
    if(ret == QDialog::Accepted){
        hydraulic_conductivity = Set_hydrogeological_parameter_window->hydraulic_conductivity(); // 获取渗透系数
        specific_yield=  Set_hydrogeological_parameter_window->specific_yield(); // 获取给水度
        flow.hydraulic_conductivity(hydraulic_conductivity);
        flow.specific_yield(specific_yield);
    }
    delete Set_hydrogeological_parameter_window;
}


void Randow_flow_Window::on_solve_FDM_clicked()
{
    // 开始时间
    auto start = std::chrono::high_resolution_clock::now();

    solve();

    // 记录结束时间
    auto end = std::chrono::high_resolution_clock::now();

    // 计算时间差
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // 输出执行时间
    ui->textBrowser->append("用时" + QString::number(duration) + "毫秒\n");

}

void Randow_flow_Window::on_doubleSpinBox_rain_valueChanged(double arg1)
{
    QString str = QString::number(arg1/ (1000 * 365));
    flow.source_sink_expectation(arg1 / (1000 * 365));  // 一年的降雨量期望转化为一天的
    ui->textBrowser_rain_function->clear();  // 重置展示降雨量函数的框
    flow.clear_list_source_sink_term();  // 重置降雨量波动列表
    ui->textBrowser_rain_function->append(str);
}

void Randow_flow_Window::on_spinBox_t_length_valueChanged(int arg1)
{
    flow.t_length(arg1);
}

void Randow_flow_Window::on_random_new_wave_clicked()
{
    if(ui->spinBox_t_length->value() != 0) flow.t_length(ui->spinBox_t_length->value());
    flow.random_source_sink_term();
    std::vector<std::vector<double>> list_source_sink_term = flow.share_list_source_sink_term();
    QString str = QString::number(ui->doubleSpinBox_rain->value()/ (1000 * 365));
    for (unsigned long long i = 0; i < list_source_sink_term.size(); i++) {
        if(list_source_sink_term[i][0] == 0){
            str += " + " + QString::number(list_source_sink_term[i][1]) + "sin((2pi / " + QString::number(list_source_sink_term[i][2]) + ") * t)";
        }
        else if(list_source_sink_term[i][0] == 1){
            str += " + " + QString::number(list_source_sink_term[i][1]) + "cos((2pi / " + QString::number(list_source_sink_term[i][2]) + ") * t)";
        }
    };
    ui->textBrowser_rain_function->clear();
    ui->textBrowser_rain_function->append(str);
}

void Randow_flow_Window::on_new_wave_clicked() // 此函数用于打手动新建一个波动的子窗口并设置数值
{
    if(ui->spinBox_t_length->value() != 0) flow.t_length(ui->spinBox_t_length->value());
    set_new_wave *set_new_wave_window = new set_new_wave;
    set_new_wave_window->setAttribute(Qt::WA_DeleteOnClose); // 对话框关闭时自动删除
    set_new_wave_window->set_text(ui->doubleSpinBox_rain->value());
    connect(set_new_wave_window, &set_new_wave::wave_info, this, &Randow_flow_Window::get_wave_info); // 绑定信号和槽
    set_new_wave_window->setModal(false);
    set_new_wave_window->show();
}

void Randow_flow_Window::on_delete_wave_clicked()
{
    if(flow.share_list_source_sink_term().size() == 0) return;
    else{
        flow.del_last_list_source_sink_term();
        std::vector<std::vector<double>> list_source_sink_term = flow.share_list_source_sink_term();
        QString str = QString::number(ui->doubleSpinBox_rain->value()/ (1000 * 365));
        for (unsigned long long i = 0; i < list_source_sink_term.size(); i++) {
            if(list_source_sink_term[i][0] == 0){
                str += " + " + QString::number(list_source_sink_term[i][1]) + "sin((2pi / " + QString::number(list_source_sink_term[i][2]) + ") * t)";
            }
            else if(list_source_sink_term[i][0] == 1){
                str += " + " + QString::number(list_source_sink_term[i][1]) + "cos((2pi / " + QString::number(list_source_sink_term[i][2]) + ") * t)";
            }
        };
        ui->textBrowser_rain_function->clear();
        ui->textBrowser_rain_function->append(str);
    }
}

void Randow_flow_Window::on_draw_solve_line_clicked()
{
    chart_head->removeSeries(series_head); // 清除原有图表
    chart_head->removeAxis(axis_x);
    chart_head->removeAxis(axis_head);
    series_head->clear();

    QString title = "数值解，空间差分步长为" +QString::number(flow.show_sl()) + "时间差分步长为" + QString::number(flow.show_st()) + "，绘图时刻为第" + QString::number(ui->spinBox_time->value()) + "时刻。";
    ui->graphicsView->setChart(chart_head);
    chart_head->setTitle(title);
    series_head->setName("水头曲线: 数值解");
    int a = ui->spinBox_time->value();
    double x = 0.0;
    double min_h = 100.0;
    double max_h = 0.0;
    for (double h : solve_fdm.row(a)) {
        series_head->append(x, h);
        x += flow.show_sl();
        if(h < min_h) min_h = h;
        if(h > max_h) max_h = h;
    }

    axis_x->setRange(0, flow.show_xl());
    axis_x->setLabelFormat("%.1f"); // 标签格式
    axis_x->setTickCount(11);
    axis_x->setMinorTickCount(2);
    axis_x->setTitleText("X轴(m)");

    axis_head->setRange(min_h, max_h);
    axis_head->setLabelFormat("%.1f"); // 标签格式
    axis_head->setTickCount(11);
    axis_head->setMinorTickCount(2);
    axis_head->setTitleText("水头(m)");

    chart_head->addSeries(series_head); // 更新图表
    chart_head->addAxis(axis_x, Qt::AlignBottom);
    chart_head->addAxis(axis_head, Qt::AlignLeft);
    series_head->attachAxis(axis_x);
    series_head->attachAxis(axis_head);

}

void Randow_flow_Window::on_draw_solve_line_location_clicked()
{
    chart_head->removeSeries(series_head); // 清除原有图表
    chart_head->removeAxis(axis_x);
    chart_head->removeAxis(axis_head);
    series_head->clear();

    QString title = "数值解，空间差分步长为" +QString::number(flow.show_sl()) + "时间差分步长为" + QString::number(flow.show_st()) + "，绘图位置为第" + QString::number(ui->spinBox_X->value()) + "位置。";
    ui->graphicsView->setChart(chart_head);
    chart_head->setTitle(title);
    series_head->setName("水头曲线: 数值解");
    int a = ui->spinBox_X->value();
    double x = 0.0;
    double min_h = 100.0;
    double max_h = 0.0;
    for (double h : solve_fdm.col(a)) {
        series_head->append(x, h);
        x += flow.show_st();
        if(h < min_h) min_h = h;
        if(h > max_h) max_h = h;
    }

    axis_x->setRange(0, flow.show_tl());
    axis_x->setLabelFormat("%.1f"); // 标签格式
    axis_x->setTickCount(11);
    axis_x->setMinorTickCount(2);
    axis_x->setTitleText("时间轴(d)");

    axis_head->setRange(min_h, max_h);
    axis_head->setLabelFormat("%.1f"); // 标签格式
    axis_head->setTickCount(11);
    axis_head->setMinorTickCount(2);
    axis_head->setTitleText("水头(m)");

    chart_head->addSeries(series_head); // 更新图表
    chart_head->addAxis(axis_x, Qt::AlignBottom);
    chart_head->addAxis(axis_head, Qt::AlignLeft);
    series_head->attachAxis(axis_x);
    series_head->attachAxis(axis_head);
}

void Randow_flow_Window::on_time_field_figure_clicked()
{
    chart_W->removeSeries(series_W); // 清除原有图表
    chart_W->removeAxis(axis_W);
    chart_W->removeAxis(axis_w);
    series_W->clear();

    QString title = "源汇项时域图像";
    ui->graphicsView_W->setChart(chart_W);
    chart_W->setTitle(title);
    series_W->setName("源汇项波动曲线");
    int n = floor(flow.show_tl() / flow.show_st()) + 1;
    double t = 0.0;
    double min_h = 100.0;
    double max_h = 0.0;
    for(int i = 0; i < n; i++){
        double h =flow.source_sink_term(t);
        series_W->append(t, h);
        t += flow.show_st();
        if(h < min_h) min_h = h;
        if(h > max_h) max_h = h;
    }

    axis_w->setRange(0, flow.show_tl());
    axis_w->setGridLineVisible(false);
    axis_w->setLabelFormat("%.1f"); // 标签格式
    axis_w->setTickCount(6);
    axis_w->setMinorTickCount(1);
    axis_w->setTitleText("T轴(d)");

    axis_W->setRange(min_h, max_h);
    axis_W->setGridLineVisible(false);
    axis_W->setLabelFormat("%.5f"); // 标签格式
    axis_W->setTickCount(6);
    axis_W->setMinorTickCount(1);
    axis_W->setTitleText("水头(m)");

    chart_W->addSeries(series_W); // 更新图表
    chart_W->addAxis(axis_w, Qt::AlignBottom);
    chart_W->addAxis(axis_W, Qt::AlignLeft);
    series_W->attachAxis(axis_w);
    series_W->attachAxis(axis_W);
}

void Randow_flow_Window::on_frequency_field_figure_clicked()
{
    chart_W->removeSeries(series_W); // 清除原有图表
    chart_W->removeAxis(axis_W);
    chart_W->removeAxis(axis_w);
    series_W->clear();

    QString title = "源汇项频域图像";
    ui->graphicsView_W->setChart(chart_W);
    chart_W->setTitle(title);
    series_W->setName("源汇项频谱曲线");

    flow.set_n_m();
    Eigen::VectorXd Amplitude(flow.show_n());
    double t = 0.0;
    // 离散化源汇项赋值
    for(int i = 0; i < flow.show_n(); i++){
        Amplitude[i] = flow.source_sink_term(t);
        t += flow.show_st();
    }

    // 快速傅里叶变换振幅
    Eigen::VectorXd Amplitude_fft = flow.fast_fourier_transfrom(Amplitude, flow.show_n());

    // 找到最大振幅值，以便设置绘图坐标轴
    double max_A = Amplitude_fft.maxCoeff();
    ui->graphicsView_W->setChart(chart_W); // 这段必须放在前面
    ui->textBrowser->append("最大振幅为:" + QString::number(max_A));

    double w = 0.0;
    for (double W : Amplitude_fft) {
        series_W->append(w, W);
        w += (1/ flow.show_tl());
    }

    // 绘图
    axis_w->setRange(0, 1 / (2*flow.show_st())); // 转换为取一半的频率坐标
    axis_w->setLabelFormat("%.2f"); // 标签格式
    axis_w->setTickCount(6);
    axis_w->setMinorTickCount(1);
    axis_w->setTitleText("频率(Hz)");

    axis_W->setRange(0, max_A);
    axis_W->setLabelFormat("%.5f"); // 标签格式
    axis_W->setTickCount(6);
    axis_W->setMinorTickCount(1);
    axis_W->setTitleText("振幅(m)");

    chart_W->addSeries(series_W); // 更新图表
    chart_W->addAxis(axis_w, Qt::AlignBottom);
    chart_W->addAxis(axis_W, Qt::AlignLeft);
    series_W->attachAxis(axis_w);
    series_W->attachAxis(axis_W);
}


void Randow_flow_Window::on_draw_solve_line_location_fft_clicked()
{
    chart_head->removeSeries(series_head); // 清除原有图表
    chart_head->removeAxis(axis_x);
    chart_head->removeAxis(axis_head);
    series_head->clear();

    QString title = "数值解，空间差分步长为" +QString::number(flow.show_sl()) + "时间差分步长为" + QString::number(flow.show_st()) + "，绘图位置为第" + QString::number(ui->spinBox_X->value()) + "位置。";
    ui->graphicsView->setChart(chart_head);
    chart_head->setTitle(title);
    series_head->setName("水头曲线: 数值解(频域)");

    int a = ui->spinBox_X->value();
    // 快速傅里叶变换振幅
    Eigen::VectorXd Amplitude_fft = flow.fast_fourier_transfrom(solve_fdm.col(a), flow.show_n());
    // 找到最大振幅值，以便设置绘图坐标轴
    double max_A = Amplitude_fft.maxCoeff();
    double w = 0.0;
    for (double W : Amplitude_fft) {
        series_head->append(w, W);
        w += (1/ flow.show_tl());
    }

    axis_x->setRange(0,  1 / (2*flow.show_st())); // 转换为取一半的频率坐标
    axis_x->setLabelFormat("%.2f"); // 标签格式
    axis_x->setTickCount(11);
    axis_x->setMinorTickCount(2);
    axis_x->setTitleText("频率(Hz)");

    axis_head->setRange(0, max_A);
    axis_head->setLabelFormat("%.4f"); // 标签格式
    axis_head->setTickCount(11);
    axis_head->setMinorTickCount(2);
    axis_head->setTitleText("振幅(m)");

    chart_head->addSeries(series_head); // 更新图表
    chart_head->addAxis(axis_x, Qt::AlignBottom);
    chart_head->addAxis(axis_head, Qt::AlignLeft);
    series_head->attachAxis(axis_x);
    series_head->attachAxis(axis_head);

}

