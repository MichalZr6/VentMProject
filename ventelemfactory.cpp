#include "ventelemfactory.h"

VentElemFactory::VentElemFactory(const std::filesystem::path &path_to_file_listing_elements, VentElemFileType type)
    : _listing_file_path {path_to_file_listing_elements}, _ftype{type}
{
    basic_file_check();
}

const std::vector<std::unique_ptr<VentElem>> *VentElemFactory::elements() const
{
    return &_elements;
}

void VentElemFactory::add(std::unique_ptr<VentElem> &&ve)
{
    ve->set_properties();
    if(ve->is_valid())
        _elements.push_back(std::move(ve));
    else
    {
        if(!ve->is_empty())
        {
            auto e = ve->get_errors();
            if(!e.isEmpty())
                log_critical("VentElemFactory::add() error:\t"+ e);
        }
    }
}

QString VentElemFactory::get_errors() const
{
    return _errors.get_up_to_max_printable();
}

bool VentElemFactory::basic_file_check()
{
    if(!std::filesystem::exists(_listing_file_path))
    {
        log_critical(QString::fromStdWString(L"VentElemFactory::basic_file_check() error: Plik "+ _listing_file_path.wstring()
                                             +L" nie istnieje lub jest w innej lokalizacji."));
        return false;
    }

    if(std::filesystem::is_directory(_listing_file_path))
    {
        log_critical(QString::fromStdWString(L"VentElemFactory::basic_file_check() error: Ścieżka "+ _listing_file_path.wstring()
                                             +L" jest katalogiem."));
        return false;
    }

    auto ext = _listing_file_path.extension().string();
    if(ext == ".xls")
    {
        log_critical("VentElemFactory::basic_file_check() error: Format pliku zestawienia to .xls. "
                     "Należy zapisać go w nowszym formacie, np. .xlsx.");
        return false;
    }

    return true;
}

void VentElemFactory::log_critical(QString crit_msg)
{
    _is_valid = false;
    _errors.log_error(crit_msg, errType::CRITICAL);
}

void VentElemFactory::log_warning(QString msg)
{
    _errors.log_error(msg, errType::WARNING);
}

