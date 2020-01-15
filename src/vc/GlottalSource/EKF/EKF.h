#ifndef VC_VC_EKF_EKF_H
#define VC_VC_EKF_EKF_H

#include <Eigen/Dense>
#include <utility>

std::pair<double, double> optlf_Tp_and_alpha(Eigen::Ref<const Eigen::ArrayXd> gfd, int period, double Ee, double Tp0, double Te, double a0min, double a0max, double a0step);

double optlf_alpha(Eigen::Ref<const Eigen::ArrayXd> gfd, int period, double Ee, double Tp, double Te, double a0min, double a0max, double a0step);

double optlf_epsilon(Eigen::Ref<const Eigen::ArrayXd> gfd, int period, double Ee, double Ta, double Te, double e0min, double e0max, double e0step);

double lf_ho(double alpha, int k, int period, double Ee, double Tp, double Te);
double da_lf_ho(double alpha, int k, int period, double Ee, double Tp, double Te);

double lf_hr(double eps, int k, int period, double Ee, double Ta, double Te);
double de_lf_hr(double eps, int k, int period, double Ee, double Ta, double Te);

#endif // VC_VC_EKF_EKF_H
