#include "ptable.hpp"

#define confly_api         confly::
#define ptable_record_api  confly::ptable_record::
#define ptable_api         confly::ptable::


// -----------------------------------------
// ptable_record

ptable_record_api ptable_record()
    : _I(0)
    , _J(0)
    , _P(0.0)
    , _V(0)
    , _PLb(0.0)
    , _PUb(0.0) 
{}

ptable_record_api ptable_record(
    const integer_type& _cI,
    const integer_type& _cJ,
    const real_type& _cP,
    const integer_type& _cV,
    const real_type& _cPLb,
    const real_type& _cPUb)
    : _I(_cI)
    , _J(_cJ)
    , _P(_cP)
    , _V(_cV)
    , _PLb(_cPLb)
    , _PUb(_cPUb) 
{}

ptable_record_api ~ ptable_record()
{}

bool ptable_record_api operator < (
    const ptable_record& _Right) 
    const
{
    return _I < _Right._I;
}

bool ptable_record_api read(
    std::istream& _S)
{
    if (! _S.good())
        return false;

    _S >> _I >> _J
        >> _P >> _V
        >> _PLb >> _PUb;

    return _S.good();
}

void ptable_record_api write(
    std::ostream& _S, 
    char _NewLine)
    const
{
    _S << 
    _I << CHARSP <<
    _J << CHARSP <<
    _P << CHARSP <<
    _V << CHARSP <<
    _PLb << CHARSP <<
    _PUb << _NewLine;
}

auto ptable_record_api value() const
    ->integer_type
{
    return _V;
}

auto ptable_record_api lower() const
    ->real_type
{
    return _PLb;
}

auto ptable_record_api upper() const
    ->real_type
{
    return _PUb;
}

// -----------------------------------------
// ptable

ptable_api ptable() 
    : _NCat(0) 
{
}

ptable_api ~ ptable() {}


void ptable_api insert(
    const ptable_record& _Rec)
{
    _PtableRecords.insert(
        std::make_pair(
            _Rec._I, _Rec));

    if (_Rec._I>_NCat)
        _NCat=_Rec._I;
}

void ptable_api write(
    std::ostream& _S)
    const
{
    for (auto I=_PtableRecords
        .cbegin(), E=_PtableRecords
            .cend(); I!=E; ++I) 
            I->second.write(
                _S, CHARNL);
}

bool ptable_api read(
    std::istream& _S)
{
    debug_msg<3>(
        M_READ,
        M_PTABLE);

    while (_S.good())
    {
        ptable_record _Rec;

        if (! _Rec.read(_S))
        {
            if (_S.eof())
                return true;

            return false;
        }

        _S.ignore(1);

        this->insert(_Rec);

        debug_msg<4>(_Rec);
    }

    return true;
}

auto ptable_api lookup(
    const integer_type& _I, 
    const real_type& _Ckey)
    const ->integer_type
{
    integer_type
        _IDX(_I);

    if (_IDX>_NCat)
        _IDX%=(_NCat+1);

    auto _PairII=
        _PtableRecords
        .equal_range(_IDX);

    for (auto I=_PairII.first, 
        E=_PairII.second;
            I!=E; ++I)
    {
        if (I->second.lower()
            <=_Ckey 
        && I->second.upper()
            >=_Ckey)
        return I->second.value();
    }

    return 0.0;
}

auto ptable_api get_ncat() 
    const 
    ->integer_type
{
    return _NCat;
}

void ptable_api set_ncat(
    const integer_type& _Val)
{
    _NCat=_Val;
}

auto ptable_api size() const
    ->size_t
{
    return _PtableRecords
        .size();
}

bool confly_api load_ptable(
    ptable& _PTable,
    const std::string& _Fname)
{// load a pre-existing ptable
    std::ifstream _Fin(_Fname);

    if (! _Fin.good())
    {
        flush_msg(std::cout, 
            M_ERROR,
            M_OPEN,
            M_PTABLE,
            M_FILE);

        return false;
    }

    if (! _PTable.read(_Fin))
    {
        flush_msg(std::cout, 
            M_ERROR,
            M_READ,
            M_PTABLE,
            M_FILE);

        return false;
    }

    return true;
}

bool confly_api create_ptable(
    const std::string& _ScriptFname,
    const std::string& _Fname,
    const real_type& D_CK, 
    const real_type& V_CK, 
    const real_type& js_CK)
{
    //  BN (CK) parameters: 
    //  bound D_CK, 
    //  variance V_CK, 
    //  small-count threshold js_CK

    cnectR _Connector(
        _ScriptFname);

    if (! _Connector.good())
        return false;

    if (! _Connector.call(
        R_CRTPTABLE, _Fname,
            D_CK, V_CK, 
                js_CK))
        return false;

    return true;
}



