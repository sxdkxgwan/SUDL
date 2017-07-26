#include "layer.h"

namespace sub_dl {

class WordEmbeddingLayer : public Layer {

public:

	WordEmbeddingLayer(int output_dim) {
		_output_dim = output_dim;
	}
	
	void _forward(Layer* pre_layer) {
		std::vector<matrix_double>().swap(_data);
		if (pre_layer->_type != INPUT) {
			exit(1);
		}
		_seq_len = pre_layer->_data.size();
		for (int i = 0; i < _seq_len; i++) {
			int word_id = pre_layer->_data[i][0][0];
			int idx = 0;
			matrix_double feature(1, _output_dim);
			while (word_id > 0) {
				feature[0][idx ++] = word_id % 2;
				word_id /= 2;
			}
			for (; idx < _output_dim; idx++) {
				feature[0][idx] = 0;
			}
			_data.push_back(feature);
		}
	}
	void _backward(Layer* nxt_layer) {}
	void display() {}
	void _update_gradient(int opt_type, double learning_rate) {}

};

}
