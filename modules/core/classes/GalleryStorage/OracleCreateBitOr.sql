
-- Create BIT_OR aggregate function (requires Oracle 9i or higher)
-- Optional in a G2 Oracle instllation.. see BIT_OR comments in OracleStorage.class

create or replace type BIT_OR_IMPL as object (
  val NUMBER,
  static function ODCIAggregateInitialize(sctx IN OUT BIT_OR_IMPL)
    return number,
  member function ODCIAggregateIterate(self IN OUT BIT_OR_IMPL,
    value IN number) return number,
  member function ODCIAggregateTerminate(self IN OUT BIT_OR_IMPL,
    returnValue OUT number, flags IN number) return number,
  member function ODCIAggregateMerge(self IN OUT BIT_OR_IMPL,
    ctx2 IN BIT_OR_IMPL) return number
);

create or replace type body BIT_OR_IMPL is
  static function ODCIAggregateInitialize(sctx IN OUT BIT_OR_IMPL)
    return number is
  begin
    sctx := BIT_OR_IMPL(0);
    return ODCIConst.Success;
  end;
  member function ODCIAggregateIterate(self IN OUT BIT_OR_IMPL,
    value IN number) return number is
  begin
    self.val := (self.val + value) - BitAND(self.val, value);
    return ODCIConst.Success;
  end;
  member function ODCIAggregateTerminate(self IN OUT BIT_OR_IMPL,
    returnValue OUT number, flags IN number) return number is
  begin
    returnValue := self.val;
    return ODCIConst.Success;
  end;
  member function ODCIAggregateMerge(self IN OUT BIT_OR_IMPL,
    ctx2 IN BIT_OR_IMPL) return number is
  begin
    self.val := (self.val + ctx2.val) - BitAND(self.val, ctx2.val);
    return ODCIConst.Success;
  end;
end;
/

create or replace function BIT_OR(input number) return number
  parallel_enable aggregate using BIT_OR_IMPL;
