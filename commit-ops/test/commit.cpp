#include "commit-ops.h"
#include "debug/log.h"
#include "debug/debug-help.h"
#include "commit.pb.h"
commit_helper *p, *s;
get_commit_by_loc_handler_t handler = [](commit_handle_t handle)
{
    verbose("invoked");
    if (handle == 1)
        return p;
    else if (handle >= 2)
    {
        return s;
    }
    else if (handle == 0)
    {
        return (commit_helper *)NULL;
    }
    else
        panic("unkown commit");
};
int main()
{

    p = new commit_helper(0, &handler);
    auto fns = gen_tmp_files(10);

    for (uint64_t i = 1; i <= fns.size(); i++)
    {
        verbose("file name:%s", fns[i - 1].c_str());
        p->commit_file(file_desc{fns[i - 1], i});
        // if (handler.target_type().name()[0] == '3')
        //   panic("handler name:%s", handler.target_type().name());
    }
    auto ps = p->commit();
    s = new commit_helper(1, &handler);

    for (uint64_t i = 1; i <= fns.size(); i++)
    {
        // verbose("file name:%s", fns[i - 1].c_str());
        s->commit_file(file_desc{fns[i - 1], i});
    }
    auto b = s->commit();
    data::commit cmt;
    cmt.ParseFromString(b);
    cmt.CheckInitialized();
    EQ(cmt.parent(), 1);
    for (auto i : cmt.files())
    {
        EQ(i.second.has_binary(), false);
        EQ(i.second.parent(), 1);
    }
    auto ss = new commit_helper(2, &handler);
    verbose("ss added");

    for (uint64_t i = 1; i <= fns.size(); i++)
    {
        // verbose("file name:%s", fns[i - 1].c_str());
        ss->commit_file(file_desc{fns[i - 1], i});
    }
    b = ss->commit();
    data::commit cmt1;
    cmt1.ParseFromString(b);
    cmt1.CheckInitialized();
    EQ(cmt1.parent(), 2);
    for (auto i : cmt1.files())
    {
        EQ(i.second.has_binary(), false);
        EQ(i.second.parent(), 1);
    }

}
