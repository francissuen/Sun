#include "../src/args.h"
#include "../src/logger.h"

//./fstest -d -n hello -c 100 world fsfs
int args_test(int argc, char** argv)
{
    FS_SUN_FUNC_LOG(argc, argv[0]);

    using fs::Sun::args;
    args arg;
    arg.register_namedArg('d', args::enmType::BOOL);
    arg.register_namedArg('n', args::enmType::STRING);
    arg.register_namedArg('c', args::enmType::INT);

    try
    {
	arg.parse(argc, argv);

	std::cout << "d: " << arg.named_arg<bool>('d') << std::endl;
	std::cout << "n: " << arg.named_arg<char*>('n') << std::endl;
	std::cout << "c: " << arg.named_arg<int>('c') << std::endl;
	
	std::cout << "has named arg d: " << arg.has_named_arg('d') << std::endl;

	const size_t uaSize = arg.unnamed_arg_size();
	std::cout << "unnamed arg size: " << uaSize << std::endl;

	for(size_t i = 0; i < uaSize; i++)
	{
	    std::cout << "ua " << i << ": " << arg.unnamed_arg(i) << std::endl;	
	}


	arg.unnamed_arg(uaSize); /** will throw an excetpion here */
    }
    catch(const fs::Sun::string& err)
    {
	FS_SUN_FUNC_ERR("the format of argv is wrong", err);
    }

    return 0;
}
