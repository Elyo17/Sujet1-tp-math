#include <Eigen/Dense>
#include <vector>

struct IATrainData
{
    std::vector<double> _x;
    double              _y;
};

std::vector<double> computeHyperplanCoefficients(const std::vector<IATrainData> &data)
{
    if (data.empty())
        return {};

    const int N = static_cast<int>(data.front()._x.size());

    if (data.size() < static_cast<size_t>(N + 1))
        return {};

    for (const auto& d : data)
        assert(d._x.size() == static_cast<size_t>(N));

    Eigen::MatrixXd X(data.size(), N + 1);
    Eigen::VectorXd Y(data.size());

    for (size_t i = 0; i < data.size(); ++i)
    {
        X(i, 0) = 1.0;

        for (size_t j = 0 ; j < data[i]._x.size() ; j++)
            X(i, j + 1) = data[i]._x[j];

        Y(i) = data[i]._y;
    }

    Eigen::VectorXd r =
        (X.transpose() * X)
            .ldlt()
            .solve(X.transpose() * Y);

    return std::vector<double>(r.data(), r.data() + r.size());
}
