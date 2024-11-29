#include "lsm_tree.h"

extern "C"
{
    void *lsm_tree_create()
    {
        return new lsm_tree();
    }

    void lsm_tree_destroy(void *tree)
    {
        delete static_cast<lsm_tree *>(tree);
    }

    void lsm_tree_put(void *tree, const char *key, const char *value)
    {
        static_cast<lsm_tree *>(tree)->put(std::string(key), std::string(value));
    }

    const char *lsm_tree_get(void *tree, const char *key)
    {
        static std::string result;
        result = static_cast<lsm_tree *>(tree)->get(std::string(key));
        return result.c_str();
    }

    void lsm_tree_remove(void *tree, const char *key)
    {
        static_cast<lsm_tree *>(tree)->remove(std::string(key));
    }

    void lsm_tree_drop_table(void *tree)
    {
        static_cast<lsm_tree *>(tree)->drop_table();
    }
}
