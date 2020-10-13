//*****************************************************************************
// Copyright 2017-2020 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//*****************************************************************************

#pragma once

#include <memory>
#include <ostream>
#include <vector>

#include "ngraph/ngraph.hpp"

using namespace std;

namespace tensorflow {
namespace ngraph_bridge {

class Executable {
 public:
  Executable();
  virtual ~Executable();

  /// \param outputs vector of ngraph::runtime::Tensor used as outputs
  /// \param inputs vector of ngraph::runtime::Tensor used as inputs
  /// \returns true if iteration is successful, false otherwise
  virtual bool call(
      const vector<shared_ptr<ngraph::runtime::Tensor> >& outputs,
      const vector<shared_ptr<ngraph::runtime::Tensor> >& inputs) = 0;

  /// \brief Executes a single iteration of a Function.
  /// \param outputs vector of ngraph::runtime::Tensor used as outputs
  /// \param inputs vector of ngraph::runtime::Tensor used as inputs
  /// \returns true if iteration is successful, false otherwise
  bool call_with_validate(
      const vector<shared_ptr<ngraph::runtime::Tensor> >& outputs,
      const vector<shared_ptr<ngraph::runtime::Tensor> >& inputs);

  /// \brief Validates a Function.
  /// \param outputs vector of ngraph::runtime::Tensor used as outputs
  /// \param inputs vector of ngraph::runtime::Tensor used as inputs
  void validate(const vector<shared_ptr<ngraph::runtime::Tensor> >& outputs,
                const vector<shared_ptr<ngraph::runtime::Tensor> >& inputs);

  /// \brief Query the input Parameters
  /// \returns an ngraph::op::ParameterVector of all input parameters
  const ngraph::ParameterVector& get_parameters() const;

  /// \brief Query the output Results
  /// \returns an ngraph::ResultVector of all input parameters
  const ngraph::ResultVector& get_results() const;

  /// \brief Get the preferred pipeline_depth for this executable
  /// \returns  preferred pipeline_depth
  virtual size_t get_preferred_pipeline_depth() const;

  /// \brief Save this compiled Executable to an output stream.
  ///    Saved stream may be read with Backend::load
  virtual void save(ostream& output_stream);

  /// \brief Create an input Tensor
  /// \param input_index The index position in the input Parameter vector. This
  /// would be the same
  /// order of Parameters passed into the inputs in the call() method.
  /// \returns A Tensor
  virtual shared_ptr<ngraph::runtime::Tensor> create_input_tensor(
      size_t input_index);

  /// \brief Create an input Tensor
  /// \param input_index The index position in the input Parameter vector. This
  /// would be the same
  /// order of Parameters passed into the inputs in the call() method.
  /// \param memory_pointer A pointer to a buffer used for this tensor. The size
  /// of the buffer
  ///     must be sufficient to contain the tensor. The lifetime of the buffer
  ///     is the
  ///     responsibility of the caller and must outlive the created Tensor.
  /// \returns A Tensor
  virtual shared_ptr<ngraph::runtime::Tensor> create_input_tensor(
      size_t input_index, void* memory_pointer);

  /// \brief Create an output Tensor
  /// \param output_index The index position in the output Result vector. This
  /// would be the same
  /// order of Results passed into the outputs in the call() method.
  /// \returns A Tensor
  virtual shared_ptr<ngraph::runtime::Tensor> create_output_tensor(
      size_t output_index);

  /// \brief Create an output Tensor
  /// \param output_index The index position in the output Result vector. This
  /// would be the same
  /// order of Results passed into the outputs in the call() method.
  /// \param memory_pointer A pointer to a buffer used for this tensor. The size
  /// of the buffer
  ///     must be sufficient to contain the tensor. The lifetime of the buffer
  ///     is the
  ///     responsibility of the caller and must outlive the created Tensor.
  /// \returns A Tensor
  virtual shared_ptr<ngraph::runtime::Tensor> create_output_tensor(
      size_t output_index, void* memory_pointer);

  /// \brief Create a vector of input Tensors
  /// \param input_index The index position in the input Parameter vector. This
  /// would be the same
  /// order of Parameters passed into the inputs in the call() method.
  /// \param pipeline_depth The number of stages in the input pipeline. For
  /// double-buffered input
  /// you would specify pipeline_depth=2
  /// \returns A vector of Tensors, one for each stage of the pipeline
  virtual vector<shared_ptr<ngraph::runtime::Tensor> > create_input_tensor(
      size_t input_index, size_t pipeline_depth);

  /// \brief Create a vector of input Tensors
  /// \param input_index The index position in the input Parameter vector. This
  /// would be the same
  /// order of Parameters passed into the inputs in the call() method.
  /// \param pipeline_depth The number of stages in the input pipeline. For
  /// double-buffered input
  /// you would specify pipeline_depth=2
  /// \param memory_pointers A vector of pointers to buffers used for this
  /// tensors. The size of
  ///     the buffer must be sufficient to contain the tensor. The lifetime of
  ///     the buffers is the
  ///     responsibility of the caller and must outlive the created Tensor.
  /// \returns A vector of Tensors, one for each stage of the pipeline
  virtual vector<shared_ptr<ngraph::runtime::Tensor> > create_input_tensor(
      size_t input_index, size_t pipeline_depth, vector<void*> memory_pointers);

  /// \brief Create a vector of output Tensors
  /// \param output_index The index position in the output Result vector. This
  /// would be the same
  ///                     order of Results passed into the outputs in the call()
  ///                     method.
  /// \param pipeline_depth The number of stages in the output pipeline. For
  /// double-buffered
  ///                       output you would specify pipeline_depth=2
  /// \returns A vector of Tensors, one for each stage of the pipeline
  virtual vector<shared_ptr<ngraph::runtime::Tensor> > create_output_tensor(
      size_t output_index, size_t pipeline_depth);

  /// \brief Create a vector of output Tensors
  /// \param output_index The index position in the output Result vector. This
  /// would be the same
  ///                     order of Results passed into the outputs in the call()
  ///                     method.
  /// \param pipeline_depth The number of stages in the output pipeline. For
  /// double-buffered
  ///                       output you would specify pipeline_depth=2
  /// \param memory_pointers A vector of pointers to buffers used for this
  /// tensors. The size of
  ///     the buffer must be sufficient to contain the tensor. The lifetime of
  ///     the buffers is the
  ///     responsibility of the caller and must outlive the created Tensor.
  /// \returns A vector of Tensors, one for each stage of the pipeline
  virtual vector<shared_ptr<ngraph::runtime::Tensor> > create_output_tensor(
      size_t output_index, size_t pipeline_depth,
      vector<void*> memory_pointers);

  /// \brief Return the output batch size
  /// \param input_batch_size The batch size of the input is needed to compute
  ///                         the output batch size
  /// \param device The device should be known to compute the output batch size
  /// \returns Output batch size of the graph
  virtual size_t get_batch_size(size_t input_batch_size, std::string device) const = 0;

 protected:
  /// \brief Called at the end of compile to the values to be returned by
  /// get_parameters
  ///        and get_results
  /// \param func The function with Results fully resolved.
  void set_parameters_and_results(const ngraph::Function& func);

  ngraph::ParameterVector m_parameters;
  ngraph::ResultVector m_results;
};

}  // namespace ngraph_bridge
}  // namespace tensorflow
