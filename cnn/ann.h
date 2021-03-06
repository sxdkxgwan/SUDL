#ifndef ANN_H_
#define ANN_H_

#include <fstream>
#include "matrix.h"
#include "loss_layer.h"

namespace sub_dl {

template <typename T>
class ANN {

public:

    std::vector<std::vector<matrix_double> > train_x_feature;
    std::vector<matrix_double> train_y_label;

    std::vector<Layer*> _layers;

    void build_ann(std::vector<Layer*> layers) {
        _layers = layers;
    }

    void _forward(std::vector<matrix_double> data) {
        // Layer* pre_layer = new DataFeedLayer(data);
        Layer* pre_layer;
		for (auto layer : _layers) {
            layer->_forward(pre_layer);
            pre_layer = layer;
        }
    }

    double _backward(const matrix_double& label) {
        // MeanSquareLossLayer* nxt_layer = new MeanSquareLossLayer(label);
        Layer* nxt_layer = new T(label);
        nxt_layer->_forward(_layers.back());
        nxt_layer->_backward(NULL);

        double cost = 0.0;
        cost += nxt_layer->_data[0].sum();
        //Layer* nxt_layer = _layers.back();
        //((MeanSquareLossLayer*)nxt_layer)->_set_data(label);
        for (int j = _layers.size() - 1; j >= 0; j--) {
            _layers[j]->_backward(nxt_layer);
            nxt_layer = _layers[j];
			std::cout << nxt_layer->_type << std::endl;
        }
        return cost;

    }

    void _update_gradient() {
        for (auto layer : _layers) {
            layer->_update_gradient(SGD, -0.1);
        }
    }
    
    void load_mnist_data(const char*file_name) {
        std::ifstream fis(file_name);
        int id, r, g, b, label;
        char comma;
        while (fis >> label) {
            matrix_double label_data(1, 10);
            label_data[0][label] = 1;
            std::vector<matrix_double> x_feature;
            matrix_double value(28, 28);
            // std::cout << label << std::endl;
            for (int i = 0; i < 28; i++) {
                for (int j = 0; j < 28; j++) {
                    int v;
                    fis >> comma;
                    fis >> v;
                    value[i][j] = v;
                }
            }
            matrix_double t(1, 49);
            for (int i = 0; i < 7; i++) {
                for (int j = 0; j < 7; j++) {
                    t[0][i * 7 + j] = value.local(i * 4, j * 4, 4, 4).sum() / 1600.0;
                }
            }
            t._display("feature");
            x_feature.push_back(t);
            train_x_feature.push_back(x_feature);
            train_y_label.push_back(label_data);
        }
        
        // fis.close();
        std::cout << "Load Data" << std::endl;
    }

    void load_data(const char*file_name) {
        std::ifstream fis(file_name);
        int id, r, g, b, label;
        char comma;
        while (fis >> label) {
            matrix_double label_data(1, 3);
            label_data[0][label] = 1;
            std::vector<matrix_double> x_feature;
            matrix_double value(1, 4);
            // std::cout << label << std::endl;
            for (int i = 0; i < 4; i++) {
                double v;
                fis >> comma;
                fis >> v;
                value[0][i] = v;
            }
            x_feature.push_back(value);
            train_x_feature.push_back(x_feature);
            train_y_label.push_back(label_data);
        }
        for (int i = 0; i < train_x_feature.size(); i++) {
            int v = rand() % (train_x_feature.size());
            int s = rand() % (train_x_feature.size());
            //std::swap(train_x_feature[v], train_x_feature[s]);
            //std::swap(train_y_label[v], train_y_label[s]);
        }
        // fis.close();
        std::cout << "Load Data " << train_x_feature.size() << std::endl;
    }

    int merge(const matrix_double& data) {
        double v = -100;
        int id = 0;
        for (int i = 0; i < data._y_dim; i++) {
            if (v < data[0][i]) {
                v = data[0][i];
                id = i;
            }
        }
        return id;
    }
    
    void gradient_check(std::vector<matrix_double> batch_x_feature,
        matrix_double label) {    
        for (int i = 0; i < _layers.size(); i++) {
            if (_layers[i]->_type == FULL_CONN) {
                BaseFullConnLayer* layer = (BaseFullConnLayer*)_layers[i];
                std::cout << "--------Gradient Check Result----------" << std::endl;

                for (int i = 0; i < layer->_input_dim; i++) {
                    for (int j = 0; j < layer->_output_dim; j++) {
                        double v = layer->_full_conn_weights[i][j];
                        layer->_full_conn_weights[i][j] = v + 1.0e-4;
                        _forward(batch_x_feature);
                        Layer* nxt_layer = new T(label);
                        nxt_layer->_forward(_layers.back());
                        nxt_layer->_backward(NULL);
                        double f1 = nxt_layer->_data[0].sum();    
                        layer->_full_conn_weights[i][j] = v - 1.0e-04;
                        _forward(batch_x_feature);
                        
                        nxt_layer = new T(label);
                        nxt_layer->_forward(_layers.back());
                        nxt_layer->_backward(NULL);
                        double f2 = nxt_layer->_data[0].sum();    
                        std::cout << "[ " << layer->_delta_full_conn_weights[i][j] << " " << (f1 - f2) / (2.0e-4) << "], ";
						layer->_full_conn_weights[i][j] = v ;
                    }
                    std::cout << std::endl;
                }
            }
        }
    }

    void train() {
        int epoch_cnt = 100;
        int batch_size = 10;
        int tot = 150;
        for (int epoch = 0; epoch < epoch_cnt; epoch++) {
            for (int batch = 0; batch < tot / batch_size; batch++) {
                std::vector<std::vector<matrix_double> > batch_x_feature;
                std::vector<matrix_double> batch_y_label;
                for (int i = batch * batch_size; i < 
                    batch * batch_size + batch_size; i++) {
                    batch_x_feature.push_back(train_x_feature[i]);
                    batch_y_label.push_back(train_y_label[i]);
                }
                double tot = 0;
                int v1, v2;
                for (int i = 0; i < batch_x_feature.size(); i++) {
                    _forward(batch_x_feature[i]);               

                    double cost = _backward(batch_y_label[i]);
                    tot += cost;
                    v1 = merge(batch_y_label[i]);
                    v2 = merge(_layers.back()->_data[0]);
                    gradient_check(batch_x_feature[i], batch_y_label[i]);
                    _update_gradient();
                }
                std::cout << "Cost " << tot << " " << v1 << "-" << v2 << std::endl;
            }
        }
    }
};

}
#endif

