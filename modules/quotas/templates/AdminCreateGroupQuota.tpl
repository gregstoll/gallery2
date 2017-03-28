{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2>
    {g->text text="Create A New Group Quota"}
  </h2>
</div>

<div class="gbBlock">
  <h4>
    {g->text text="Group Name"}
    <span class="giSubtitle">
      {g->text text="(required)"}
    </span>
  </h4>

  <h4>
    &nbsp;&nbsp;
    {$form.groupName}
  </h4>

  <input type="hidden" name="{g->formVar var="groupId"}" value="{$AdminCreateGroupQuota.groupId}" />
    
  {if isset($form.error.groupName.exists)}
  <div class="giError">
    {g->text text="Group quota for '%s' already exists" arg1=$form.groupName}
  </div>
  {/if}

  <h4>
    {g->text text="Quota Size"}
    <span class="giSubtitle">
      {g->text text="(required)"}
    </span>
  </h4>

  <input type="text" name="{g->formVar var="form[quotaSize]"}" value="{$form.quotaSize}" />
  <script type="text/javascript">
    document.getElementById('siteAdminForm')['{g->formVar var="form[quotaSize]"}'].focus();
  </script>
  <select name="{g->formVar var="form[quotaUnit]"}">
    {html_options options=$AdminCreateGroupQuota.quotaUnitChoices selected=$form.quotaUnit}
  </select>
  {if isset($form.error.quotaSize.missing)}
  <div class="giError">
    {g->text text="You must enter a quota size."}
  </div>
  {/if}
      
  {if isset($form.error.quotaSize.tooLarge)}
  <div class="giError">
    {g->text text="Quota size must be less than 2097152 MB (2048 GB)."}
  </div>
  {/if}
   
  {if isset($form.error.quotaSize.notNumeric)}
  <div class="giError">
    {g->text text="Invalid quota size, positive numbers and decimals only."}
  </div>
  {/if}
</div>


<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit" 
         name="{g->formVar var="form[action][create]"}" 
         value="{g->text text="Create Group Quota"}"/>
  <input type="submit" class="inputTypeSubmit" 
         name="{g->formVar var="form[action][cancel]"}" 
         value="{g->text text="Cancel"}"/>
</div>

