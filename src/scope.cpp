#include <preview.h>
#include <query.h>
#include <scope.h>
#include <localization.h>

#include <iostream>
#include <sstream>
#include <fstream>

namespace sc = unity::scopes;
using namespace std;

void Scope::start(string const&) {
    config_ = make_shared<Config>();

    setlocale(LC_ALL, "");
    string translation_directory = ScopeBase::scope_directory()
            + "/../share/locale/";
    bindtextdomain(GETTEXT_PACKAGE, translation_directory.c_str());

    // Under test we set a different API root
    char *apiroot = getenv("NETWORK_SCOPE_APIROOT");
    if (apiroot) {
        config_->moviedbroot = apiroot;
    }
}

void Scope::stop() {
}

sc::SearchQueryBase::UPtr Scope::search(const sc::CannedQuery &query,
                                        const sc::SearchMetadata &metadata) {
    // Boilerplate construction of Query
    return sc::SearchQueryBase::UPtr(new Query(query, metadata, config_));
}

sc::PreviewQueryBase::UPtr Scope::preview(sc::Result const& result,
                                          sc::ActionMetadata const& metadata) {
    // Boilerplate construction of Preview
    return sc::PreviewQueryBase::UPtr(new Preview(result, metadata, config_));
}

#define EXPORT __attribute__ ((visibility ("default")))

// These functions define the entry points for the scope plugin
extern "C" {

EXPORT
unity::scopes::ScopeBase*
// cppcheck-suppress unusedFunction
UNITY_SCOPE_CREATE_FUNCTION() {
    return new Scope();
}

EXPORT
void
// cppcheck-suppress unusedFunction
UNITY_SCOPE_DESTROY_FUNCTION(unity::scopes::ScopeBase* scope_base) {
    delete scope_base;
}

}
