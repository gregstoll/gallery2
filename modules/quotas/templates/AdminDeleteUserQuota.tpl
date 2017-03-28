{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2>
    {g->text text="Delete A User Quota"}
  </h2>
</div>
    
  </div>
  
<div class="gbBlock">
  <h3>
    {g->text text="Are you sure?"}
  </h3>

  <p class="giDescription">
    {capture name="user"}<strong>{$AdminDeleteUserQuota.user.userName}</strong>{/capture}
    {g->text text="This will completely remove the %s (%s %s) user quota from Gallery. There is no undo!"
             arg1=$smarty.capture.user
             arg2=$AdminDeleteUserQuota.quotaSize
             arg3=$AdminDeleteUserQuota.quotaUnit}
  </p>
</div>

<div class="gbBlock gcBackground1">
  <input type="hidden" name="{g->formVar var="userId"}" 
         value="{$AdminDeleteUserQuota.user.id}"/>
  <input type="submit" class="inputTypeSubmit"
	 name="{g->formVar var="form[action][delete]"}" 
         value="{g->text text="Delete"}"/>
  <input type="submit" class="inputTypeSubmit"
	 name="{g->formVar var="form[action][cancel]"}" 
         value="{g->text text="Cancel"}"/>
</div>
