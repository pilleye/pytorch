#include <ATen/ATen.h>
#include <ATen/Dispatch.h>
#include <ATen/cuda/CUDASparse.h>
#include <ATen/native/Resize.h>
#include <ATen/native/sparse/cuda/SparseBlasImpl.h>

#include <c10/util/MaybeOwned.h>

namespace at {
namespace native {

Tensor& addmv_out_sparse_csr_cuda(
    const Tensor& self,
    const Tensor& mat,
    const Tensor& vec,
    const Scalar& beta,
    const Scalar& alpha,
    Tensor& result) {
  TORCH_INTERNAL_ASSERT_DEBUG_ONLY(mat.is_sparse_csr());

  TORCH_CHECK(mat.dim() == 2, "addmv: Expected mat to be 2-D");
  TORCH_CHECK(vec.dim() == 1, "addmv: Expected vec to be 1-D");

  // Preprocessing code is copied from TORCH_IMPL_FUNC(addmv_out_cuda) at
  // aten/src/ATen/native/cuda/Blas.cpp
  // It would be nice to have it unified but there were undefined symbol
  // problems, when using the same function for CUDA and SparseCsrCUDA dispatch
  // keys and structured kernel
  c10::MaybeOwned<Tensor> self_ = expand_size(self, {mat.size(0)});
  auto betaval = beta.toComplexDouble();

  if (&result != &self) {
    at::native::resize_output(result, self_->sizes());
    if (betaval != 0.0) {
      at::native::copy_(result, *self_);
    }
  }

  if (mat._nnz() == 0) {
    // shortcut for an empty matrix
    // By definition, when beta==0, values in self should be ignored. nans and
    // infs should not propagate
    if (betaval == 0.0) {
      return result.zero_();
    } else {
      return at::mul_out(
          const_cast<Tensor&>(result),
          self,
          at::native::scalar_tensor(
              beta,
              self.scalar_type(),
              c10::nullopt /* layout */,
              at::kCPU,
              c10::nullopt /* pin_memory */));
    }
  }

  sparse::impl::cuda::addmv_out_sparse_csr(mat, vec, beta, alpha, result);
  return result;
}

} // namespace native
} // namespace at
