#include "Tools/Exception/exception.hpp"
#include "Tools/Code/LDPC/Matrix_handler/LDPC_matrix_handler.hpp"

#include "Module/Encoder/LDPC/Encoder_LDPC.hpp"
#include "Module/Encoder/LDPC/From_H/Encoder_LDPC_from_H.hpp"
#include "Module/Encoder/LDPC/From_QC/Encoder_LDPC_from_QC.hpp"
#include "Module/Encoder/LDPC/From_IRA/Encoder_LDPC_from_IRA.hpp"
#include "Module/Encoder/LDPC/DVBS2/Encoder_LDPC_DVBS2.hpp"

#include "Encoder_LDPC.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Encoder_LDPC_name   = "Encoder LDPC";
const std::string aff3ct::factory::Encoder_LDPC_prefix = "enc";

Encoder_LDPC::parameters
::parameters(const std::string &prefix)
: Encoder::parameters(Encoder_LDPC_name, prefix)
{
	type = "AZCW";

	type_set.insert({"LDPC", "LDPC_H", "LDPC_DVBS2", "LDPC_QC", "LDPC_IRA"});
}

Encoder_LDPC::parameters* Encoder_LDPC::parameters
::clone() const
{
	return new Encoder_LDPC::parameters(*this);
}

void Encoder_LDPC::parameters
::register_arguments(CLI::App &app)
{
	auto p = get_prefix();

	Encoder::parameters::register_arguments(app);

	// auto h_path_option =
	CLI::add_option(app, p,
		"--h-path",
		H_path,
		"Path to the H matrix (AList or QC formated file, required by the \"LDPC_H\" encoder).")
		->check(CLI::ExistingFile)
		->group("Standard");

	// h_path_option->excludes(app.get_option("--cw-size"  )); // N_cw is H width
	// h_path_option->excludes(app.get_option("--info-bits")); // if there is no K, then H is considered regular,
	                                                     // so K is the N - H's height


	// auto g_path_option =
	CLI::add_option(app, p,
		"--g-path",
		G_path,
		"Path to the G matrix (AList or QC formated file, required by the \"LDPC\", \"LDPC_QC\" and \"LDPC_IRA\" encoders).")
		->check(CLI::ExistingFile)
		->group("Standard");

	// g_path_option->excludes(app.get_option("--cw-size"  ));
	// g_path_option->excludes(app.get_option("--info-bits"));


	CLI::add_set(app, p,
		"--h-reorder",
		H_reorder,
		{"NONE", "ASC", "DSC"},
		"Specify if the check nodes (CNs) from H have to be reordered, 'NONE': do nothing (default), 'ASC': from the"
		" smallest to the biggest CNs, 'DSC': from the biggest to the smallest CNs.",
		true)
		->group("Standard");

	CLI::add_set(app, p,
		"--g-method",
		G_method,
		{"IDENTITY", "LU_DEC"},
		"The method used to generate G from H when using 'LDPC_H' encoder type. 'LU_DEC' method generates a hollow G"
		" thanks to the LU decomposition with a guarantee to have the systematic identity (do not work with irregular"
		" matrices) when the 'IDENTITY' method generate an identity on H to get the parity part.",
		true)
		->group("Standard");


	CLI::add_option(app, p,
		"--save-g",
		G_save,
		"Path where the generated G matrix with the 'LDPC_H' encoder type will be saved.")
		->check(CLI::OutputFile())
		->group("Standard");
}

void Encoder_LDPC::parameters
::callback_arguments()
{
	if (!G_path.empty())
	{
		tools::LDPC_matrix_handler::read_matrix_size(G_path, K, N_cw);

		if (K > N_cw)
			std::swap(K, N_cw);
	}
	else if (!H_path.empty())
	{
		unsigned M;
		tools::LDPC_matrix_handler::read_matrix_size(H_path, M, N_cw);

		if (M > N_cw)
			std::swap(M, N_cw);

		K = N_cw - M; // considered as regular so M = N - K
	}

	Encoder::parameters::callback_arguments();
}

void Encoder_LDPC::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_short_name();

	Encoder::parameters::get_headers(headers, full);

	if (type == "LDPC")
		headers[p].push_back(std::make_pair("G matrix path", G_path));

	if (type == "LDPC_H" || type == "LDPC_QC")
	{
		headers[p].push_back(std::make_pair("H matrix path", H_path));
		headers[p].push_back(std::make_pair("H matrix reordering", H_reorder));
	}

	if (type == "LDPC_H")
	{
		headers[p].push_back(std::make_pair("G build method", G_method));
		if (G_save != "")
		headers[p].push_back(std::make_pair("G save path", G_save));
	}
}

template <typename B>
module::Encoder_LDPC<B>* Encoder_LDPC::parameters
::build(const tools::Sparse_matrix &G, const tools::Sparse_matrix &H) const
{
	if (type == "LDPC"    ) return new module::Encoder_LDPC         <B>(K, N_cw, G, n_frames);
	if (type == "LDPC_H"  ) return new module::Encoder_LDPC_from_H  <B>(K, N_cw, H, G_method, G_save, n_frames);
	if (type == "LDPC_QC" ) return new module::Encoder_LDPC_from_QC <B>(K, N_cw, H, n_frames);
	if (type == "LDPC_IRA") return new module::Encoder_LDPC_from_IRA<B>(K, N_cw, H, n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B>
module::Encoder_LDPC<B>* Encoder_LDPC::parameters
::build(const tools::Sparse_matrix &G, const tools::Sparse_matrix &H, const tools::dvbs2_values& dvbs2) const
{
	if (type == "LDPC_DVBS2") return new module::Encoder_LDPC_DVBS2<B>(dvbs2, n_frames);

	return build<B>(G, H);
}

template <typename B>
module::Encoder_LDPC<B>* Encoder_LDPC
::build(const parameters           &params,
        const tools::Sparse_matrix &G,
        const tools::Sparse_matrix &H)
{
	return params.template build<B>(G, H);
}

template <typename B>
module::Encoder_LDPC<B>* Encoder_LDPC
::build(const parameters           &params,
        const tools::Sparse_matrix &G,
        const tools::Sparse_matrix &H,
        const tools::dvbs2_values& dvbs2)
{
	return params.template build<B>(G, H, dvbs2);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Encoder_LDPC<B_8 >* aff3ct::factory::Encoder_LDPC::parameters::build<B_8 >(const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&) const;
template aff3ct::module::Encoder_LDPC<B_16>* aff3ct::factory::Encoder_LDPC::parameters::build<B_16>(const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&) const;
template aff3ct::module::Encoder_LDPC<B_32>* aff3ct::factory::Encoder_LDPC::parameters::build<B_32>(const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&) const;
template aff3ct::module::Encoder_LDPC<B_64>* aff3ct::factory::Encoder_LDPC::parameters::build<B_64>(const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&) const;
template aff3ct::module::Encoder_LDPC<B_8 >* aff3ct::factory::Encoder_LDPC::build<B_8 >(const aff3ct::factory::Encoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&);
template aff3ct::module::Encoder_LDPC<B_16>* aff3ct::factory::Encoder_LDPC::build<B_16>(const aff3ct::factory::Encoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&);
template aff3ct::module::Encoder_LDPC<B_32>* aff3ct::factory::Encoder_LDPC::build<B_32>(const aff3ct::factory::Encoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&);
template aff3ct::module::Encoder_LDPC<B_64>* aff3ct::factory::Encoder_LDPC::build<B_64>(const aff3ct::factory::Encoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&);

template aff3ct::module::Encoder_LDPC<B_8 >* aff3ct::factory::Encoder_LDPC::parameters::build<B_8 >(const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&, const tools::dvbs2_values&) const;
template aff3ct::module::Encoder_LDPC<B_16>* aff3ct::factory::Encoder_LDPC::parameters::build<B_16>(const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&, const tools::dvbs2_values&) const;
template aff3ct::module::Encoder_LDPC<B_32>* aff3ct::factory::Encoder_LDPC::parameters::build<B_32>(const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&, const tools::dvbs2_values&) const;
template aff3ct::module::Encoder_LDPC<B_64>* aff3ct::factory::Encoder_LDPC::parameters::build<B_64>(const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&, const tools::dvbs2_values&) const;
template aff3ct::module::Encoder_LDPC<B_8 >* aff3ct::factory::Encoder_LDPC::build<B_8 >(const aff3ct::factory::Encoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&, const tools::dvbs2_values&);
template aff3ct::module::Encoder_LDPC<B_16>* aff3ct::factory::Encoder_LDPC::build<B_16>(const aff3ct::factory::Encoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&, const tools::dvbs2_values&);
template aff3ct::module::Encoder_LDPC<B_32>* aff3ct::factory::Encoder_LDPC::build<B_32>(const aff3ct::factory::Encoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&, const tools::dvbs2_values&);
template aff3ct::module::Encoder_LDPC<B_64>* aff3ct::factory::Encoder_LDPC::build<B_64>(const aff3ct::factory::Encoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&, const tools::dvbs2_values&);
#else
template aff3ct::module::Encoder_LDPC<B>* aff3ct::factory::Encoder_LDPC::parameters::build<B>(const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&) const;
template aff3ct::module::Encoder_LDPC<B>* aff3ct::factory::Encoder_LDPC::build<B>(const aff3ct::factory::Encoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&);

template aff3ct::module::Encoder_LDPC<B>* aff3ct::factory::Encoder_LDPC::parameters::build<B>(const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&, const tools::dvbs2_values&) const;
template aff3ct::module::Encoder_LDPC<B>* aff3ct::factory::Encoder_LDPC::build<B>(const aff3ct::factory::Encoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const aff3ct::tools::Sparse_matrix&, const tools::dvbs2_values&);
#endif
// ==================================================================================== explicit template instantiation
