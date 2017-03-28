{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{g->callback type="quotas.LoadQuotas"}

{if (!$user.isGuest || $block.quotas.LoadQuotas.quotaExists)}
<div class="{$class}">
  <h3> {g->text text="Quotas"}</h3>
  {if ($block.quotas.LoadQuotas.quotaExists)}
  <div class="gbBlock">
    <table class="QuotasBlock">
      <tr>
        {if ($block.quotas.LoadQuotas.currentUsagePercent == 100)}
	<td class="QuotasUsedFull"
            style="width:100%"><div class="QuotasHolder">&nbsp;</div></td>
        {elseif ($block.quotas.LoadQuotas.currentUnusedPercent == 100)}
	<td class="QuotasUnusedNone"
            style="width:100%;"><div class="QuotasHolder">&nbsp;</div></td>
        {else}
	<td class="QuotasUsed" 
	    style="width:{$block.quotas.LoadQuotas.currentUsagePercent}%
	          "><div class="QuotasHolder">&nbsp;</div></td>
	<td class="QuotasUnused" 
	    style="width:{$block.quotas.LoadQuotas.currentUnusedPercent}%; 
		  "><div class="QuotasHolder">&nbsp;</div></td>
        {/if}
      </tr>
    </table>
  </div>
  {/if}
  {if ($block.quotas.LoadQuotas.quotaExists)}
  <p>
    {g->text text="Used: %0.2f %s (%s%%)" arg1=$block.quotas.LoadQuotas.currentUsage
	     arg2=$block.quotas.LoadQuotas.currentUsageUnit
	     arg3=$block.quotas.LoadQuotas.currentUsagePercent}
  </p>
  <p>
    {g->text text="Quota: %0.2f %s" arg1=$block.quotas.LoadQuotas.quotaValue
	     arg2=$block.quotas.LoadQuotas.quotaValueUnit}
  </p>
  {else}
  <p>
    {g->text text="Used: %0.2f %s" arg1=$block.quotas.LoadQuotas.currentUsage
             arg2=$block.quotas.LoadQuotas.currentUsageUnit}
  </p>
  <p>
    {g->text text="Quota: Unlimited"}
  </p>
  {/if}
</div>
{/if}
